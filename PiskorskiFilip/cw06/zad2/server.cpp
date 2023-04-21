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
#include <pwd.h>
#include <cstring>
#include <fstream>
#include <mqueue.h>

struct client {
    int id;
    mqd_t queue_id;
};
std::map<int, client> clients;
mqd_t in_queue_id;
std::ofstream log_file;

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
    log_file << "SERVER: " << return_current_time_and_date() << " SIGINT signal received! Exiting...\n";
    for(long unsigned int i=0; i<clients.size(); i++){
        struct my_msgbuf_stop_to_client buf{MSG_STOP};
        mq_send(clients[i].queue_id, (char *)(&buf), sizeof(buf), MSG_STOP);
        mq_close(clients[i].queue_id);
    }
    mq_close(in_queue_id);
    mq_unlink(SERVER_QUEUE_NAME);
    log_file.close();
    exit(0);
}

int main(){
    log_file.open("log_file_" + return_current_time_and_date() + ".log");

    struct sigaction sigaction1;
    sigaction1.sa_handler = sigint_handler;
    sigaction(SIGINT, &sigaction1, nullptr);

    mq_unlink(SERVER_QUEUE_NAME);
    in_queue_id = mq_open(SERVER_QUEUE_NAME, O_RDONLY | O_CREAT | O_EXCL, 0777, nullptr);
    if(in_queue_id == -1){
        std::cout << "SERVER: " << return_current_time_and_date() << " ERROR Failed to create a queue!" << "\n";
        std::cout << "SERVER: " << return_current_time_and_date() << " ERROR Reason: " << errno << "\n";
        std::cout << "SERVER: " << return_current_time_and_date() << " ERROR Exiting..." << "\n";
        log_file << "SERVER: " << return_current_time_and_date() << " ERROR Failed to create a queue!" << "\n";
        log_file << "SERVER: " << return_current_time_and_date() << " ERROR Reason: " << errno << "\n";
        log_file << "SERVER: " << return_current_time_and_date() << " ERROR Exiting..." << "\n";
        exit(0);
    }
    std::cout << "SERVER: " << return_current_time_and_date() << " Message queue created successfully. Queue id: " << in_queue_id << "\n";
    log_file << "SERVER: " << return_current_time_and_date() << " Message queue created successfully. Queue id: " << in_queue_id << "\n";

    while(1){
        struct my_msgbuf_2one_to_server buf;
        unsigned int* type = new unsigned int;
        while(1){
            while(mq_receive(in_queue_id, (char *)(&buf), 20000, type)==-1){
                std::cout << errno << "\n";
                timespec ts{};
                ts.tv_nsec = 1000000000 / tickrate;
                nanosleep(&ts, nullptr);
            }
            switch(*type) {
                case MSG_INIT: {
                    my_msgbuf_init_to_server buf2 = *((my_msgbuf_init_to_server*) &buf);
                    int client_id;
                    std::vector<int> keys = get_keys(clients);
                    if (keys.size() == 0)
                        client_id = 0;
                    else
                        client_id = (*std::max_element(keys.begin(), keys.end())) + 1;
                    struct client new_client{client_id, mq_open(buf2.data, O_WRONLY)};
                    clients.insert(std::pair<int, client>(client_id, new_client));
                    struct my_msgbuf_init_to_client buf3{MSG_INIT, client_id};
                    mq_send(clients[client_id].queue_id, (char*)&buf3, sizeof(buf3), MSG_INIT);
                    std::cout << "SERVER: " << return_current_time_and_date() << " Client " << client_id
                              << " connected. Queue name: " << buf2.data << "\n";
                    log_file << "SERVER: " << return_current_time_and_date() << " Client " << client_id
                             << " connected. Queue name: " << buf2.data << "\n";
                    break;
                }
                case MSG_STOP: {
                    my_msgbuf_stop_to_server buf3 = *((my_msgbuf_stop_to_server*) &buf);
                    mq_close(clients[buf3.data].queue_id);
                    std::cout << "SERVER: " << return_current_time_and_date() << " Client " << buf3.data
                              << " disconnected. Queue id: " << clients[buf3.data].queue_id << "\n";
                    log_file << "SERVER: " << return_current_time_and_date() << " Client " << buf3.data
                             << " disconnected. Queue id: " << clients[buf3.data].queue_id << "\n";
                    clients.erase(buf.source_id);
                    break;
                }
                case MSG_LIST: {
                    my_msgbuf_list_to_server buf4 = *((my_msgbuf_list_to_server*) &buf);
                    std::vector<int> keys = get_keys(clients);
                    struct my_msgbuf_list_to_client buf2{MSG_LIST, (int)keys.size()};
                    for (long unsigned int i = 0; i < keys.size(); i++) {
                        buf2.data[i] = keys[i];
                    }
                    mq_send(clients[buf4.data].queue_id, (char *)&buf2, sizeof(buf2), MSG_LIST);
                    break;
                }
                case MSG_2ALL: {
                    my_msgbuf_2all_to_server buf5 = *((my_msgbuf_2all_to_server*) &buf);
                    std::cout << "SERVER: " << return_current_time_and_date() << " Message received from client " << buf5.source_id
                              << " to all clients: " << buf5.message << "\n";
                    log_file << "SERVER: " << return_current_time_and_date() << " Message received from client " << buf5.source_id
                                << " to all clients: " << buf5.message << "\n";
                    for (long unsigned int i = 0; i < clients.size(); i++) {
                        struct my_msgbuf_2all_to_client buf2{MSG_2ALL, buf5.source_id, buf5.size};
                        for (int j = 0; j < buf5.size; j++) {
                            buf2.message[j] = buf5.message[j];
                        }
                        mq_send(clients[i].queue_id, (char *)&buf2, sizeof(buf2), MSG_2ALL);
                    }
                    break;
                }
                case MSG_2ONE:{
                    my_msgbuf_2one_to_server buf2 = *((my_msgbuf_2one_to_server*) &buf);
                    std::cout << "SERVER: " << return_current_time_and_date() << " Message received from client " << buf2.source_id
                              << " to client " << buf2.destination_id << ": " << buf2.message << "\n";
                    log_file << "SERVER: " << return_current_time_and_date() << " Message received from client " << buf2.source_id
                                << " to client " << buf2.destination_id << ": " << buf2.message << "\n";
                    struct my_msgbuf_2one_to_client buf3{MSG_2ONE, buf2.source_id, buf2.size};
                    strcpy(buf3.message, buf2.message);
                    mq_send(clients[buf2.destination_id].queue_id, (char *)&buf3, sizeof(buf3), MSG_2ONE);
                    break;
                }
                default:{
                    std::cout << "SERVER: " << return_current_time_and_date() << "Received uknown message type: " << *type << "\n";
                    std::cout << "SERVER: Skipping...\n";
                    log_file << "SERVER: " << return_current_time_and_date() << "Received uknown message type: " << *type << "\n";
                    log_file << "SERVER: Skipping...\n";
                }
            }
            timespec ts{};
            ts.tv_nsec = 1000000000 / tickrate;
            nanosleep(&ts, nullptr);
        }
    }
}