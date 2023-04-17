#include "./constants.h"
#include <sys/msg.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <chrono>
#include <map>
#include <vector>
#include <iostream>

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

vector<int> get_keys(map<int, client> clients){
    vector<int> result;
    for (std::map<int, client>::const_iterator it = clients.begin(); it != clients.end(); it++) {
        result.push_back(it->first);
    }
    return result;
}

int main(){
    int in_queue_id = msgget(ftok("$HOME", proj_id), IPC_CREAT | IPC_EXCL | 0777);
    if(in_queue_id == -1){
        std::cout << "SERVER: ERROR Failed to create a queue!" << "\n"
        std::cout << "SERVER: ERROR Reason: " << errno << "\n";
        std::cout << "SERVER: ERROR Exiting..." << "\n";
        exit(0);
    }
    std::cout << "SERVER: Message queue created successfully. Queue id: " << in_queue_id << "\n";
    std::map<int, client> clients;
    char buffer[COMM_MAX_LENGTH];
    int read_chars;
    while(1){
        read_chars = 0;
        // Wait for the init signal
        read_chars = msgrcv(in_queue_id, buffer, COMM_MAX_LENGTH, -MSG_INIT, MSG_NOERROR | IPC_NOWAIT);
        while(read_chars > 0){
            int client_queue_id = &((int*)buffer);
            int client_id = std::max(get_keys(clients)) + 1;
            std::cout << "SERVER: " << std::string return_current_time_and_date() << " New client! Client queue id: " << client_queue_id << ", client id: " << client_id << "\n";
            struct client new_one;
            new_one.id = client_id;
            new_one.queue_id = client_queue_id;
            clients.push_back(new_one);
            struct msgbuf buf;
            buf.mtype = MSG_INIT;
            buf.mtext = &((char*)(&client_id));
            msgsnd(new_one.queue_id, buf, sizeof(int), 0);
            read_chars = msgrcv(in_queue_id, buffer, COMM_MAX_LENGTH, -MSG_INIT, MSG_NOERROR | IPC_NOWAIT);
        }
        read_chars = 0;
        // Wait for the stop signal
        read_chars = msgrcv(in_queue_id, buffer, COMM_MAX_LENGTH, -MSG_STOP, MSG_NOERROR | IPC_NOWAIT);
        while(read_chars > 0){



            read_chars = msgrcv(in_queue_id, buffer, COMM_MAX_LENGTH, -MSG_STOP, MSG_NOERROR | IPC_NOWAIT);
        }
        read_chars = 0;
        // Wait for the list signal
        read_chars = msgrcv(in_queue_id, buffer, COMM_MAX_LENGTH, -MSG_LIST, MSG_NOERROR | IPC_NOWAIT);
        while(read_chars > 0){



            read_chars = msgrcv(in_queue_id, buffer, COMM_MAX_LENGTH, -MSG_LIST, MSG_NOERROR | IPC_NOWAIT);
        }
        read_chars = 0;
        // Wait for the 2one signal
        read_chars = msgrcv(in_queue_id, buffer, COMM_MAX_LENGTH, -MSG_2ONE, MSG_NOERROR | IPC_NOWAIT);
        while(read_chars > 0){



            read_chars = msgrcv(in_queue_id, buffer, COMM_MAX_LENGTH, -MSG_2ONE, MSG_NOERROR | IPC_NOWAIT);
        }
        read_chars = 0;
        // Wait for the 2all signal
        read_chars = msgrcv(in_queue_id, buffer, COMM_MAX_LENGTH, -MSG_2ALL, MSG_NOERROR | IPC_NOWAIT);
        while(read_chars > 0){



            read_chars = msgrcv(in_queue_id, buffer, COMM_MAX_LENGTH, -MSG_2ALL, MSG_NOERROR | IPC_NOWAIT);
        }
        struct timespec ts;
        ts.tv_nsec = 1000000000 / tickrate;
        nanosleep(ts, nullptr);
    }
}