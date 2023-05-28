#include "./constants.h"
#include <cerrno>
#include <ctime>
#include <chrono>
#include <map>
#include <vector>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <signal.h>
#include <cstring>
#include <fstream>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <thread>
#include <arpa/inet.h>
#include <sys/un.h>


struct client {
    int id;
    int client_socket_fd;
};
std::map<int, client> clients;
int port;
std::string UNIX_socket_path;
int local_socket_fd;
int inet4_socket_fd;
int epollfd;
std::ofstream log_file;
struct epoll_event event, events[2];

std::string return_current_time_and_date()
{
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
    return ss.str();
}

std::vector<int> get_keys(const std::map<int, client>& clients){
    std::vector<int> result;
    result.reserve(clients.size());
    for (auto & client : clients) {
        result.push_back(client.first);
    }
    return result;
}

void sigint_handler(int sig){
    std::cout << "SERVER: " << return_current_time_and_date() << " SIGINT signal received! Exiting...\n";
    for(long unsigned int i=0; i<clients.size(); i++){
        struct my_msgbuf_stop_to_client buf{MSG_STOP};
        send(clients[i].client_socket_fd, (char *)(&buf), sizeof(buf), 0);
        shutdown(clients[i].client_socket_fd, SHUT_RDWR);
        close(clients[i].client_socket_fd);
    }
    shutdown(local_socket_fd, SHUT_RDWR);
    shutdown(inet4_socket_fd, SHUT_RDWR);
    close(local_socket_fd);
    close(inet4_socket_fd);
    unlink(UNIX_socket_path.c_str());
    close(epollfd);
    std::cout << "SERVER: " << return_current_time_and_date() << " Server stopped\n";
    exit(0);
}

void listening_thread_function(){
    std::cout << "SERVER: " << return_current_time_and_date() << " Server is listening on port " << port << " and on UNIX socket " << UNIX_socket_path << "\n";

    struct my_msgbuf_2one_to_server buf;
    while(1){
        int nfds = epoll_wait(epollfd, events, 4, -1);
        std::cout << "SERVER: epoll returned " << nfds << ". \n";

        if (nfds == -1) {
            std::cout << "SERVER: " << return_current_time_and_date() << " Error while waiting for events\n";
            exit(1);
        }
        for(int i = 0; i<nfds; ++i) {
            if(events[i].data.fd == local_socket_fd || events[i].data.fd == inet4_socket_fd){
                std::cout << "SERVER: " << return_current_time_and_date() << " New connection on socket " << events[i].data.fd << "\n";
                int client_socket_fd = accept(events[i].data.fd, nullptr, nullptr);
                recv(client_socket_fd, (char *)(&buf), sizeof(buf), 0);
                if(client_socket_fd == -1){
                    std::cout << "SERVER: " << return_current_time_and_date() << " Error while accepting new connection\n";
                    std::cout << "SERVER: Reason: " << strerror(errno) << "\n";
                    exit(1);
                }
                std::cout << "SERVER: " << return_current_time_and_date() << " Client accepted. Client socket: " << client_socket_fd << "\n";
                int client_id;
                std::vector<int> keys = get_keys(clients);
                if (keys.size() == 0)
                    client_id = 0;
                else
                    client_id = (*std::max_element(keys.begin(), keys.end())) + 1;
                struct client new_client{client_id, client_socket_fd};
                clients.insert(std::pair<int, client>(client_id, new_client));
                std::cout << "SERVER: " << return_current_time_and_date() << " Client added to clients map\n";
                struct my_msgbuf_init_to_client* buf3 = new my_msgbuf_init_to_client{MSG_INIT, client_id};
                send(client_socket_fd, (char *)(buf3), sizeof(buf3), 0);
                std::cout << "SERVER: " << return_current_time_and_date() << " MSG_INIT sent to client\n";
                event.events = EPOLLIN;
                event.data.fd = client_socket_fd;
                if(epoll_ctl(epollfd, EPOLL_CTL_ADD, client_socket_fd, &event) == -1){
                    std::cout << "SERVER: " << return_current_time_and_date() << " Error while adding client socket to epoll\n";
                    exit(1);
                }
                std::cout << "SERVER: " << return_current_time_and_date() << " Client socket added to epoll\n";
            }
            else{
                recv(events[i].data.fd, (char *)(&buf), sizeof(buf), 0);
                unsigned int type = buf.type;
                switch(type) {
                    case MSG_STOP: {
                        my_msgbuf_stop_to_server buf3 = *((my_msgbuf_stop_to_server*) &buf);
                        shutdown(clients[buf3.data].client_socket_fd, SHUT_RDWR);
                        close(clients[buf3.data].client_socket_fd);
                        std::cout << "SERVER: " << return_current_time_and_date() << " Client " << buf3.data
                                  << " disconnected." << "\n";
                        clients.erase(buf.source_id);
                        break;
                    }
                    case MSG_LIST: {
                        my_msgbuf_list_to_server buf4 = *((my_msgbuf_list_to_server*) &buf);
                        std::cout << "SERVER: " << return_current_time_and_date() << " Client " << buf4.data
                                  << " requested list of clients.\n";
                        std::vector<int> keys = get_keys(clients);
                        struct my_msgbuf_list_to_client buf2{MSG_LIST, (int)keys.size()};
                        memcpy(buf2.data, keys.data(), keys.size() * sizeof(int));
                        send(clients[buf4.data].client_socket_fd, (char *)&buf2, sizeof(buf2), 0);
                        break;
                    }
                    case MSG_2ALL: {
                        my_msgbuf_2all_to_server buf5 = *((my_msgbuf_2all_to_server*) &buf);
                        std::cout << "SERVER: " << return_current_time_and_date() << " Message received from client " << buf5.source_id
                                  << " to all clients: " << buf5.message << "\n";
                        for (long unsigned int i = 0; i < clients.size(); i++) {
                            struct my_msgbuf_2all_to_client buf2{MSG_2ALL, buf5.source_id, buf5.size};
                            strcpy(buf2.message, buf5.message);
                            send(clients[i].client_socket_fd, (char *)&buf2, sizeof(buf2), 0);
                        }
                        break;
                    }
                    case MSG_2ONE:{
                        my_msgbuf_2one_to_server buf2 = *((my_msgbuf_2one_to_server*) &buf);
                        std::cout << "SERVER: " << return_current_time_and_date() << " Message received from client " << buf2.source_id
                                  << " to client " << buf2.destination_id << ": " << buf2.message << "\n";
                        struct my_msgbuf_2one_to_client buf3{MSG_2ONE, buf2.source_id, buf2.size};
                        strcpy(buf3.message, buf2.message);
                        send(clients[buf2.destination_id].client_socket_fd, (char *)&buf3, sizeof(buf3), 0);
                        break;
                    }
                    default:{
                        std::cout << "SERVER: " << return_current_time_and_date() << "Received uknown message type: " << type << "\n";
                        std::cout << "SERVER: Skipping...\n";
                    }
                }
            }
        }
    }
}

int main(int argc, char* argv[]){
    if(argc != 3){
        std::cout << "SERVER: Wrong number of arguments! Required: 2, got: " << argc - 1 << "\n";
        exit(0);
    }
    port = atoi(argv[1]);
    UNIX_socket_path = std::string(argv[2]);

    struct sigaction sigaction1;
    sigaction1.sa_handler = sigint_handler;
    sigaction(SIGINT, &sigaction1, nullptr);

    std::cout << "SERVER: " << return_current_time_and_date() << "Server started, PID:" << getpid() << "\n";

    local_socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(local_socket_fd == -1){
        std::cout << "SERVER: " << return_current_time_and_date() << " ERROR Failed to create a local socket!" << "\n";
        std::cout << "SERVER: " << return_current_time_and_date() << " ERROR Reason: " << strerror(errno) << "\n";
        std::cout << "SERVER: " << return_current_time_and_date() << " ERROR Exiting..." << "\n";
        exit(0);
    }
    std::cout << "SERVER: " << return_current_time_and_date() << " Local socket created successfully. Socket id: " << local_socket_fd << "\n";

    inet4_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(inet4_socket_fd == -1){
        std::cout << "SERVER: " << return_current_time_and_date() << " ERROR Failed to create an IPv4 socket!" << "\n";
        std::cout << "SERVER: " << return_current_time_and_date() << " ERROR Reason: " << strerror(errno) << "\n";
        std::cout << "SERVER: " << return_current_time_and_date() << " ERROR Exiting..." << "\n";
        exit(0);
    }
    std::cout << "SERVER: " << return_current_time_and_date() << " IPv4 socket created successfully. Socket id: " << inet4_socket_fd << "\n";

    struct sockaddr_un local_address;
    memset(&local_address, 0, sizeof(local_address));
    local_address.sun_family = AF_UNIX;
    strcpy(local_address.sun_path, UNIX_socket_path.c_str());
    if(bind(local_socket_fd, (struct sockaddr *) &local_address, sizeof(local_address)) == -1){
        std::cout << "SERVER: " << return_current_time_and_date() << " ERROR Failed to bind a local socket!" << "\n";
        std::cout << "SERVER: " << return_current_time_and_date() << " ERROR Reason: " << strerror(errno) << "\n";
        std::cout << "SERVER: " << return_current_time_and_date() << " ERROR Exiting..." << "\n";
        exit(0);
    }
    std::cout << "SERVER: " << return_current_time_and_date() << " Local socket bound successfully.\n";

    struct sockaddr_in inet4_address;
    inet4_address.sin_family = AF_INET;
    inet4_address.sin_port = htons(port);
    inet4_address.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(inet4_socket_fd, (struct sockaddr *) &inet4_address, sizeof(inet4_address)) == -1){
        std::cout << "SERVER: " << return_current_time_and_date() << " ERROR Failed to bind an IPv4 socket!" << "\n";
        std::cout << "SERVER: " << return_current_time_and_date() << " ERROR Reason: " << strerror(errno) << "\n";
        std::cout << "SERVER: " << return_current_time_and_date() << " ERROR Exiting..." << "\n";
        exit(0);
    }
    std::cout << "SERVER: " << return_current_time_and_date() << " IPv4 socket bound successfully.\n";

    if(listen(local_socket_fd, 10) == -1){
        std::cout << "SERVER: " << return_current_time_and_date() << " ERROR Failed to listen on a local socket!" << "\n";
        std::cout << "SERVER: " << return_current_time_and_date() << " ERROR Reason: " << strerror(errno) << "\n";
        std::cout << "SERVER: " << return_current_time_and_date() << " ERROR Exiting..." << "\n";
        exit(0);
    }
    if(listen(inet4_socket_fd, 10) == -1){
        std::cout << "SERVER: " << return_current_time_and_date() << " ERROR Failed to listen on a local socket!" << "\n";
        std::cout << "SERVER: " << return_current_time_and_date() << " ERROR Reason: " << strerror(errno) << "\n";
        std::cout << "SERVER: " << return_current_time_and_date() << " ERROR Exiting..." << "\n";
        exit(0);
    }
    epollfd = epoll_create1(0);
    if(epollfd == -1){
        std::cout << "SERVER: " << return_current_time_and_date() << " Error while creating epoll\n";
        exit(1);
    }

    event.events = EPOLLIN;
    event.data.fd = local_socket_fd;
    if(epoll_ctl(epollfd, EPOLL_CTL_ADD, local_socket_fd, &event) == -1){
        std::cout << "SERVER: " << return_current_time_and_date() << " Error while adding local socket to epoll\n";
        exit(1);
    }
    event.data.fd = inet4_socket_fd;
    if(epoll_ctl(epollfd, EPOLL_CTL_ADD, inet4_socket_fd, &event) == -1){
        std::cout << "SERVER: " << return_current_time_and_date() << " Error while adding inet4 socket to epoll\n";
        exit(1);
    }
    std::cout << "SERVER: " << return_current_time_and_date() << " epoll successfully created " << "\n";

    std::cout << "SERVER: " << return_current_time_and_date() << " Starting a listening thread...\n";
    std::thread listening_thread(listening_thread_function);

    listening_thread.join();

    exit(0);
}
