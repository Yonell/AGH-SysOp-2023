#include <iostream>
#include "./constants.h"
#include <sys/msg.h>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <signal.h>
#include <pwd.h>

int in_queue_id;
int to_server_queue_id;
int client_id;

void sigint_handler(int sig){
    msgctl(in_queue_id, IPC_RMID, nullptr);
    exit(0);
}

void send_init_to_server(){
    struct my_msgbuf_init_to_server buf{MSG_INIT, MSG_INIT, in_queue_id};
    msgsnd(to_server_queue_id, &buf, sizeof(int) + sizeof(buf.data), MSG_NOERROR | IPC_NOWAIT);
}

void receive_init_from_server(){
    struct my_msgbuf_init_to_client buf;
    msgrcv(in_queue_id, &buf, sizeof(int) + sizeof(buf.data), MSG_INIT, 0);
    client_id = buf.data;
}

void send_stop_to_server() {
    my_msgbuf_stop_to_server buf;
    buf.mtype = MSG_STOP;
    buf.type = MSG_STOP;
    buf.data = client_id;
    msgsnd(to_server_queue_id, &buf, sizeof(int)*2, 0);
    msgctl(in_queue_id, IPC_RMID, nullptr);
}

void print_list_from_server(){
    my_msgbuf_list_to_server buf;
    buf.mtype = MSG_LIST;
    buf.type = MSG_LIST;
    buf.data = client_id;
    std::cout << "CLIENT: Sending list request to server\n";
    msgsnd(to_server_queue_id, &buf, sizeof(int)*2, 0);
    std::cout << "CLIENT: List request sent\n";
    std::cout << "CLIENT: Waiting for list from server\n";
    my_msgbuf_list_to_client buf2;
    int read_chars = msgrcv(in_queue_id, &buf2, COMM_MAX_LENGTH, MSG_LIST, MSG_NOERROR);
    std::cout << "CLIENT: List received\n";
    std::cout << "CLIENT: List of clients: " << "\n";
    for(int i = 0; i < buf2.size; i++){
        std::cout << "CLIENT: " << buf2.data[i] << "\n";
    }
    std::cout << "CLIENT: End of the list\n";
}

void send_2one_message(){
    my_msgbuf_2one_to_server buf;
    buf.mtype = MSG_2ONE;
    buf.type = MSG_2ONE;
    buf.source_id = client_id;
    std::cout << "CLIENT: Enter the id of the client you want to send a message to: ";
    int dest_id;
    std::cin >> dest_id;
    buf.destination_id = dest_id;
    std::cout << "CLIENT: Enter the message: ";
    std::string string_buffer;
    std::getline(std::cin, string_buffer);
    memcpy(buf.message, string_buffer.c_str(), string_buffer.size());
    buf.message[string_buffer.size()] = 0;
    buf.size = strlen(buf.message);
    msgsnd(to_server_queue_id, &buf, sizeof(int)*4 + COMM_MAX_LENGTH, 0);
}

void send_2all_message(){
    my_msgbuf_2all_to_server buf;
    buf.mtype = MSG_2ALL;
    buf.type = MSG_2ALL;
    buf.source_id = client_id;
    std::cout << "CLIENT: Enter the message: ";
    std::string string_buffer;
    std::getline(std::cin, string_buffer);
    std::cout << "buff" << string_buffer << "buff\n";
    std::cout << "buff" << std::cin.good() << "buff\n";
    memcpy(buf.message, string_buffer.c_str(), string_buffer.size());
    buf.message[string_buffer.size()] = 0;
    buf.size = strlen(buf.message);
    msgsnd(to_server_queue_id, &buf, sizeof(int)*3 + COMM_MAX_LENGTH, 0);
}

void read_queue() {
    my_msgbuf_2one_to_client buf;
    int read_chars = msgrcv(in_queue_id, &buf, COMM_MAX_LENGTH, MSG_2ONE, MSG_NOERROR | IPC_NOWAIT);
    while(read_chars > 0){
        std::cout << "CLIENT: Private message received from " << buf.source_id << ": " << std::string(buf.message) << "\n";
        read_chars = msgrcv(in_queue_id, &buf, COMM_MAX_LENGTH, MSG_2ONE, MSG_NOERROR | IPC_NOWAIT);
    }
    my_msgbuf_2all_to_client buf2;
    read_chars = msgrcv(in_queue_id, &buf2, COMM_MAX_LENGTH, MSG_2ALL, MSG_NOERROR | IPC_NOWAIT);
    while(read_chars > 0){
        std::cout << "CLIENT: Public message received from " << buf2.source_id << ": " << std::string(buf2.message) << "\n";
        read_chars = msgrcv(in_queue_id, &buf2, COMM_MAX_LENGTH, MSG_2ALL, MSG_NOERROR | IPC_NOWAIT);
    }
    my_msgbuf_stop_to_client buf3;
    read_chars = msgrcv(in_queue_id, &buf3, 0, MSG_STOP, MSG_NOERROR | IPC_NOWAIT);
    if(read_chars != -1){
        std::cout << "CLIENT: Stop message received! Exiting...\n";
        msgctl(in_queue_id, IPC_RMID, nullptr);
        exit(0);
    }
}

int main(){
    struct passwd *pw = getpwuid(getuid());
    const char *homedir = pw->pw_dir;
    struct sigaction sigaction1;
    sigaction1.sa_handler = sigint_handler;
    sigaction(SIGINT, &sigaction1, nullptr);

    to_server_queue_id = msgget(ftok(homedir, proj_id), 0);
    if(to_server_queue_id == -1){
        std::cout << "CLIENT: ERROR Failed to get a queue!" << "\n";
        std::cout << "CLIENT: ERROR Reason: " << errno << "\n";
        std::cout << "CLIENT: ERROR Exiting..." << "\n";
        exit(0);
    }
    std::cout << "CLIENT: To-server message queue got successfully. Queue id: " << to_server_queue_id << "\n";

    srand(time(NULL));
    key_t key = ftok(homedir, (char)(rand()%255));
    std::cout << key << " " << errno << "\n";
    in_queue_id = msgget(key, IPC_CREAT | IPC_EXCL | IPC_RMID | 0777);
    if(in_queue_id == -1){
        std::cout << "CLIENT: ERROR Failed to create a queue!" << "\n";
        std::cout << "CLIENT: ERROR Reason: " << errno << "\n";
        std::cout << "CLIENT: ERROR Exiting..." << "\n";
        exit(0);
    }
    std::cout << "CLIENT: Input message queue created successfully. Queue id: " << in_queue_id << "\n";

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