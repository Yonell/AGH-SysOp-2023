#include "./constants.h"
#include <sys/msg.h>
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

struct client {
    int id;
    int queue_id;
};

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
std::map<int, client> clients;
int in_queue_id;
std::ofstream log_file;

void sigint_handler(int sig){
    std::cout << "SERVER: " << return_current_time_and_date() << " SIGINT signal received! Exiting...\n";
    log_file << "SERVER: " << return_current_time_and_date() << " SIGINT signal received! Exiting...\n";
    for(int i=0; i<clients.size(); i++){
        struct my_msgbuf_stop_to_client buf{MSG_STOP, MSG_STOP};
        msgsnd(clients[i].queue_id, &buf, 0, MSG_NOERROR | IPC_NOWAIT);
    }
    msgctl(in_queue_id, IPC_RMID, nullptr);
    log_file.close();
    exit(0);
}

void check_for_init(){
    my_msgbuf_init_to_server buf;
    int read_chars = msgrcv(in_queue_id, &buf, COMM_MAX_LENGTH, MSG_INIT, MSG_NOERROR | IPC_NOWAIT);
    while(read_chars > 0){
        int client_queue_id = buf.data;
        std::vector<int> keys = get_keys(clients);
        int client_id;
        if(keys.size() == 0)
            client_id = 0;
        else
            client_id = (*std::max_element(keys.begin(), keys.end())) + 1;
        std::cout << "SERVER: " << return_current_time_and_date() << " New client connected! Client queue id: " << client_queue_id << ", client id: " << client_id << "\n";
        log_file << "SERVER: " << return_current_time_and_date() << " New client connected! Client queue id: " << client_queue_id << ", client id: " << client_id << "\n";
        struct client new_one{client_id, client_queue_id};
        clients.insert(std::pair<int, client>(client_id, new_one));
        struct my_msgbuf_init_to_client buf2{MSG_INIT, MSG_INIT, client_id};
        msgsnd(new_one.queue_id, &buf2, sizeof(int)*2, 0);
        read_chars = msgrcv(in_queue_id, &buf, COMM_MAX_LENGTH, MSG_INIT, MSG_NOERROR | IPC_NOWAIT);
    }
}

void check_for_stop(){
    my_msgbuf_stop_to_server buf;
    int read_chars = msgrcv(in_queue_id, &buf, COMM_MAX_LENGTH, MSG_STOP, MSG_NOERROR | IPC_NOWAIT);
    while(read_chars > 0){
        int client_id = buf.data;
        std::cout << "SERVER: " << return_current_time_and_date() << " Client " << client_id << " disconnected!\n";
        log_file << "SERVER: " << return_current_time_and_date() << " Client " << client_id << " disconnected!\n";
        clients.erase(client_id);
        read_chars = msgrcv(in_queue_id, &buf, COMM_MAX_LENGTH, MSG_STOP, MSG_NOERROR | IPC_NOWAIT);
    }
}

void check_for_list(){
    my_msgbuf_list_to_server buf;
    int read_chars = msgrcv(in_queue_id, &buf, COMM_MAX_LENGTH, MSG_LIST, MSG_NOERROR | IPC_NOWAIT);
    while(read_chars>0){
        int client_id = buf.data;
        if(clients.find(client_id) == clients.end()){
            std::cout << "SERVER: " << return_current_time_and_date() << " Client with id: " << client_id << " does not exist! Skipping..." << "\n";
            log_file << "SERVER: " << return_current_time_and_date() << " Client with id: " << client_id << " does not exist! Skipping..." << "\n";
            read_chars = msgrcv(in_queue_id, &buf, COMM_MAX_LENGTH, MSG_LIST, MSG_NOERROR | IPC_NOWAIT);
            continue;
        }
        std::cout << "SERVER: " << return_current_time_and_date() << " Client " << client_id << " requested a list of connected clients!" << "\n";
        log_file << "SERVER: " << return_current_time_and_date() << " Client " << client_id << " requested a list of connected clients!" << "\n";
        std::vector<int> keys = get_keys(clients);
        std::cout << "SERVER: " << return_current_time_and_date() << " Sending a list of connected clients to client " << client_id << "!" << "\n";
        log_file << "SERVER: " << return_current_time_and_date() << " Sending a list of connected clients to client " << client_id << "!" << "\n";
        int* keys_array = new int[keys.size()];
        for(int i=0; i<keys.size(); i++){
            keys_array[i] = keys[i];
        }
        struct my_msgbuf_list_to_client buf2;
        buf2.mtype = MSG_LIST;
        buf2.type = MSG_LIST;
        buf2.size = keys.size();
        memcpy(buf2.data, keys_array, sizeof(int) * keys.size());
        msgsnd(clients[client_id].queue_id, &buf2, sizeof(int) * (keys.size()+2), 0);
        read_chars = msgrcv(in_queue_id, &buf, COMM_MAX_LENGTH, MSG_LIST, MSG_NOERROR | IPC_NOWAIT);
    }
}

void check_for_2one(){
    struct my_msgbuf_2one_to_server buf;
    int read_chars = msgrcv(in_queue_id, &buf, COMM_MAX_LENGTH, MSG_2ONE, MSG_NOERROR | IPC_NOWAIT);
    while(read_chars > 0){
        std::cout << "SERVER: " << return_current_time_and_date() << " Client " << buf.source_id << " requested to send a message to client " << buf.destination_id << "!" << "\n";
        log_file << "SERVER: " << return_current_time_and_date() << " Client " << buf.source_id << " requested to send a message to client " << buf.destination_id << "!" << "\n";
        std::cout << "SERVER: " << return_current_time_and_date() << " Message: " << buf.message << "\n";
        log_file << "SERVER: " << return_current_time_and_date() << " Message: " << buf.message << "\n";
        if(clients.find(buf.source_id) == clients.end()){
            std::cout << "SERVER: " << return_current_time_and_date() << " Client with id: " << buf.source_id << " does not exist! Skipping..." << "\n";
            log_file << "SERVER: " << return_current_time_and_date() << " Client with id: " << buf.source_id << " does not exist! Skipping..." << "\n";
            read_chars = msgrcv(in_queue_id, &buf, COMM_MAX_LENGTH, MSG_2ONE, MSG_NOERROR | IPC_NOWAIT);
            continue;
        }
        if(clients.find(buf.destination_id) == clients.end()){
            std::cout << "SERVER: " << return_current_time_and_date() << " Client with id: " << buf.destination_id << " does not exist! Skipping..." << "\n";
            log_file << "SERVER: " << return_current_time_and_date() << " Client with id: " << buf.destination_id << " does not exist! Skipping..." << "\n";
            read_chars = msgrcv(in_queue_id, &buf, COMM_MAX_LENGTH, MSG_2ONE, MSG_NOERROR | IPC_NOWAIT);
            continue;
        }
        struct my_msgbuf_2one_to_client *buf2 = (my_msgbuf_2one_to_client *) malloc(sizeof(my_msgbuf_2one_to_client));
        buf2->mtype = MSG_2ONE;
        buf2->type = MSG_2ONE;
        buf2->size = buf.size;
        buf2->source_id = buf.source_id;
        buf2->destination_id = buf.destination_id;
        strcpy(buf2->message, buf.message);
        msgsnd(clients[buf.destination_id].queue_id, buf2, sizeof(int) * 4 + COMM_MAX_LENGTH, 0);
        read_chars = msgrcv(in_queue_id, &buf, COMM_MAX_LENGTH, MSG_2ONE, MSG_NOERROR | IPC_NOWAIT);
    }
}

void check_for_2all(){
    struct my_msgbuf_2all_to_server buf;
    int read_chars = msgrcv(in_queue_id, &buf, COMM_MAX_LENGTH, MSG_2ALL, MSG_NOERROR | IPC_NOWAIT);
    while(read_chars > 0){
        std::cout << "SERVER: " << return_current_time_and_date() << " Client " << buf.source_id << " requested to send a message to all clients!" << "\n";
        log_file << "SERVER: " << return_current_time_and_date() << " Client " << buf.source_id << " requested to send a message to all clients!" << "\n";
        std::cout << "SERVER: " << return_current_time_and_date() << " Message: " << buf.message << "\n";
        log_file << "SERVER: " << return_current_time_and_date() << " Message: " << buf.message << "\n";
        if(clients.find(buf.source_id) == clients.end()){
            std::cout << "SERVER: " << return_current_time_and_date() << " Client with id: " << buf.source_id << " does not exist! Skipping..." << "\n";
            log_file << "SERVER: " << return_current_time_and_date() << " Client with id: " << buf.source_id << " does not exist! Skipping..." << "\n";
            read_chars = msgrcv(in_queue_id, &buf, COMM_MAX_LENGTH, MSG_2ALL, MSG_NOERROR | IPC_NOWAIT);
            continue;
        }
        std::vector<int> keys = get_keys(clients);
        for(int i=0; i<keys.size(); i++){
            struct my_msgbuf_2all_to_client *buf2 = (my_msgbuf_2all_to_client*) malloc(sizeof(my_msgbuf_2all_to_client));
            buf2->mtype = MSG_2ALL;
            buf2->type = MSG_2ALL;
            buf2->size = buf.size;
            buf2->source_id = buf.source_id;
            strcpy(buf2->message, buf.message);
            msgsnd(clients[keys[i]].queue_id, buf2, sizeof(int) * 3 + COMM_MAX_LENGTH, 0);
        }
        read_chars = msgrcv(in_queue_id, &buf, COMM_MAX_LENGTH, MSG_2ALL, MSG_NOERROR | IPC_NOWAIT);
    }
}

int main(){
    log_file.open("log_file_" + return_current_time_and_date() + ".log");
    struct passwd *pw = getpwuid(getuid());
    const char *homedir = pw->pw_dir;
    struct sigaction sigaction1;
    sigaction1.sa_handler = sigint_handler;
    sigaction(SIGINT, &sigaction1, nullptr);
    msgctl(0, IPC_RMID, nullptr);
    in_queue_id = msgget(ftok(homedir, proj_id), IPC_CREAT | IPC_EXCL | 0777 | IPC_RMID);
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
        // Wait for the init signal
        check_for_init();
        // Wait for the stop signal
        check_for_stop();
        // Wait for the list signal
        check_for_list();
        // Wait for the 2one signal
        check_for_2one();
        // Wait for the 2all signal
        check_for_2all();
        timespec ts{};
        ts.tv_nsec = 1000000000 / tickrate;
        nanosleep(&ts, nullptr);
    }
}