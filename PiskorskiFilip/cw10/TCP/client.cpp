#include <iostream>
#include "./constants.h"
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <signal.h>
#include <pwd.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/epoll.h>

int socket_fd;
int client_id;
std::string name;
bool local_or_inet;
int server_port;
in_addr server_address;
std::string path;

void send_stop_to_server() {
    my_msgbuf_stop_to_server buf;
    buf.type = MSG_STOP;
    buf.data = client_id;
    send(socket_fd, (char*)&buf, sizeof(buf), 0);
    shutdown(socket_fd, SHUT_RDWR);
    close(socket_fd);
}

void sigint_handler(int sig){
    send_stop_to_server();
    exit(0);
}

void send_init_to_server(){
    struct my_msgbuf_init_to_server buf{MSG_INIT};
    strcpy(buf.data, name.c_str());
    int result = send(socket_fd, (char*)&buf, sizeof(buf), 0);
    if(result == -1){
        std::cout << "CLIENT: Failed to contact the server" << "\n";
        std::cout << "CLIENT: Reason: " << strerror(errno) << "\n";
    }
}

void receive_init_from_server(){
    struct my_msgbuf_init_to_client buf;
    while(recv(socket_fd, (char*)&buf, 20000, 0)==-1);
    if(buf.type != MSG_INIT){
        std::cout << "CLIENT: Wrong mssg type: " << buf.type << "\n";
        std::cout << "CLIENT: Expected: " << MSG_INIT << "\n";
        exit(1);
    }
    client_id = buf.data;
}

void print_list_from_server(){
    my_msgbuf_list_to_server buf;
    buf.type = MSG_LIST;
    buf.data = client_id;
    std::cout << "CLIENT: Sending list request to server\n";
    send(socket_fd, (char*)&buf, sizeof(buf), 0);
    std::cout << "CLIENT: List request sent\n";
    std::cout << "CLIENT: Waiting for list from server\n";
    my_msgbuf_list_to_client buf2;
    recv(socket_fd, &buf2, sizeof(buf2), 0);
    std::cout << "CLIENT: List received\n";
    std::cout << "CLIENT: List of clients: " << "\n";
    for(int i = 0; i < buf2.size; i++){
        std::cout << "CLIENT: " << buf2.data[i] << "\n";
    }
    std::cout << "CLIENT: End of the list\n";
}

void send_2one_message(){
    my_msgbuf_2one_to_server buf;
    buf.type = MSG_2ONE;
    buf.source_id = client_id;
    std::cout << "CLIENT: Enter the id of the client you want to send a message to: ";
    int dest_id;
    std::cin >> dest_id;
    buf.destination_id = dest_id;
    std::cout << "CLIENT: Enter the message: ";
    std::string string_buffer;
    std::cin.ignore(2, '\n');
    std::getline(std::cin, string_buffer);
    strcpy(buf.message, string_buffer.c_str());
    buf.message[string_buffer.size()] = 0;
    buf.size = strlen(buf.message);
    send(socket_fd, (char*)&buf, sizeof(buf), 0);
}

void send_2all_message(){
    my_msgbuf_2all_to_server buf;
    buf.type = MSG_2ALL;
    buf.source_id = client_id;
    std::cout << "CLIENT: Enter the message: ";
    std::string string_buffer;
    std::cin.ignore(2, '\n');
    std::getline(std::cin, string_buffer);
    strcpy(buf.message, string_buffer.c_str());
    buf.message[string_buffer.size()] = 0;
    buf.size = strlen(buf.message);
    send(socket_fd, (char*)&buf, sizeof(buf), 0);
}

void read_queue() {
    my_msgbuf_2one_to_server buf;
    unsigned int* type = new unsigned int;
    timespec* timeout = new timespec;
    timeout->tv_sec = 0;
    while(recv(socket_fd, (char*)&buf, 20000, MSG_DONTWAIT) > 0) {
        *type = buf.type;
        switch (*type) {
            case MSG_2ONE: {
                std::string message(buf.message);
                std::cout << "CLIENT: Message received from " << buf.source_id << ": " << message << "\n";
                break;
            }
            case MSG_2ALL: {
                my_msgbuf_2all_to_client buf2 = *(my_msgbuf_2all_to_client *) &buf;
                std::string message(buf2.message);
                std::cout << "CLIENT: Message received from " << buf2.source_id << ": " << message << "\n";
                break;
            }
            case MSG_LIST: {
                my_msgbuf_list_to_client buf3 = *(my_msgbuf_list_to_client *) &buf;
                std::cout << "CLIENT: List of clients: " << "\n";
                for (int i = 0; i < buf3.size; i++) {
                    std::cout << "CLIENT: " << buf3.data[i] << "\n";
                }
                std::cout << "CLIENT: End of the list\n";
                break;
            }
            case MSG_STOP: {
                std::cout << "CLIENT: Server stopped\n";
                raise(SIGINT);
                break;
            }
        }
    }
}

int main(int argc, char* argv[]){
    if(argc != 4 && argc != 5){
        std::cout << "CLIENT: Wrong number of arguments! Required: 3/4, got: " << argc - 1 << "\n";
        exit(0);
    }
    name = std::string(argv[1]);
    if(strcmp(argv[2], "local") == 0){
        local_or_inet = 0;
        if(argc == 4){
            path = std::string(argv[3]);
        } else {
            std::cout << "CLIENT: Wrong number of arguments! Required: 3, got: " << argc - 1 << "\n";
            exit(0);
        }
    } else if(strcmp(argv[2], "inet") == 0){
        local_or_inet = 1;
        if(argc == 5){
            inet_aton(argv[3], &server_address);
            server_port = atoi(argv[4]);
        } else {
            std::cout << "CLIENT: Wrong number of arguments! Required: 4, got: " << argc - 1 << "\n";
            exit(0);
        }
    } else {
        std::cout << "CLIENT: Wrong type of connection! Required: local/inet, got: " << argv[2] << "\n";
        exit(0);
    }


    struct sigaction sigaction1;
    sigaction1.sa_handler = sigint_handler;
    sigaction(SIGINT, &sigaction1, nullptr);

    std::cout << "CLIENT: " << "Client started, PID:" << getpid() << "\n";

    if(!local_or_inet){
        if((socket_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1){
            std::cout << "CLIENT: Error while creating socket\n";
            std::cout << "CLIENT: Reason: " << strerror(errno) << "\n";
            exit(0);
        }
        sockaddr_un serv_address;
        serv_address.sun_family = AF_UNIX;
        strcpy(serv_address.sun_path, path.c_str());
        if(connect(socket_fd, (sockaddr*)&serv_address, sizeof(serv_address)) == -1){
            std::cout << "CLIENT: Error while connecting to the server\n";
            std::cout << "CLIENT: Reason: " << strerror(errno) << "\n";
            exit(0);
        }
    } else {
        if((socket_fd = socket(AF_INET, SOCK_STREAM, 0))==-1){
            std::cout << "CLIENT: Error while creating socket\n";
            std::cout << "CLIENT: Reason: " << strerror(errno) << "\n";
            exit(0);
        }
        sockaddr_in serv_address;
        serv_address.sin_family = AF_INET;
        serv_address.sin_addr = server_address;
        serv_address.sin_port = htons(server_port);
        if(connect(socket_fd, (sockaddr*)&serv_address, sizeof(serv_address)) == -1){
            std::cout << "CLIENT: Error while connecting to the server\n";
            std::cout << "CLIENT: Reason: " << strerror(errno) << "\n";
            exit(0);
        }

    }

    std::cout << "CLIENT: Sending init signal to the server..." << "\n";
    send_init_to_server();
    std::cout << "CLIENT: Waiting for the server response..." << "\n";
    receive_init_from_server();
    std::cout << "CLIENT: Client id received: " << client_id << "\n";

    std::cout << "CLIENT: Starting the chat..." << "\n";
    std::string command;
    std::cout << "CLIENT: Enter a command: "; std::cin >> command;
    while(command != "stop"){
        if(command == "list"){
            print_list_from_server();
        } else if(command == "2all"){
            send_2all_message();
        } else if(command == "2one"){
            send_2one_message();
        } else if(command == "read_queue"){
            read_queue();
        } else {
            std::cout << "CLIENT: Unknown command. Try again..." << "\n";
        }
        std::cout << "CLIENT: Enter a command: "; std::cin >> command;
    }
    std::cout << "CLIENT: Sending stop signal to the server..." << "\n";
    send_stop_to_server();
    std::cout << "CLIENT: Exiting..." << "\n";
    return 0;
}