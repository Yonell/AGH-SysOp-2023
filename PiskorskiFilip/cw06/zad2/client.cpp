#include <iostream>
#include "./constants.h"
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <signal.h>
#include <pwd.h>
#include <mqueue.h>

mqd_t in_queue_id;
mqd_t to_server_queue_id;
int client_id;
char* client_queue_name;

void send_stop_to_server() {
    my_msgbuf_stop_to_server buf;
    buf.type = MSG_STOP;
    buf.data = client_id;
    mq_send(to_server_queue_id, (char*)&buf, sizeof(buf), MSG_STOP);
    mq_close(to_server_queue_id);
    mq_close(in_queue_id);
    mq_unlink(client_queue_name);
}

void sigint_handler(int sig){
    send_stop_to_server();
    mq_close(in_queue_id);
    mq_unlink(client_queue_name);
    mq_close(to_server_queue_id);
    exit(0);
}

void send_init_to_server(){
    struct my_msgbuf_init_to_server buf{MSG_INIT};
    strcpy(buf.data, client_queue_name);
    int result = mq_send(to_server_queue_id, (char*)&buf, sizeof(buf), MSG_INIT);
    if(result == -1){
        std::cout << "CLIENT: Failed to contact the server" << "\n";
        std::cout << "CLIENT: Reason: " << errno << "\n";
    }
}

void receive_init_from_server(){
    struct my_msgbuf_init_to_client buf;
    while(mq_receive(in_queue_id, (char*)&buf, 20000, nullptr)==-1);
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
    mq_send(to_server_queue_id, (char*)&buf, sizeof(buf), MSG_LIST);
    std::cout << "CLIENT: List request sent\n";
   /* std::cout << "CLIENT: Waiting for list from server\n";
    my_msgbuf_list_to_client buf2;
    mq_receive(in_queue_id, &buf2, sizeof(buf2), 0);
    std::cout << "CLIENT: List received\n";
    std::cout << "CLIENT: List of clients: " << "\n";
    for(int i = 0; i < buf2.size; i++){
        std::cout << "CLIENT: " << buf2.data[i] << "\n";
    }
    std::cout << "CLIENT: End of the list\n";*/
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
    memcpy(buf.message, string_buffer.c_str(), string_buffer.size()+1);
    buf.message[string_buffer.size()] = 0;
    buf.size = strlen(buf.message);
    mq_send(to_server_queue_id, (char*)&buf, sizeof(buf), MSG_2ONE);
}

void send_2all_message(){
    my_msgbuf_2all_to_server buf;
    buf.type = MSG_2ALL;
    buf.source_id = client_id;
    std::cout << "CLIENT: Enter the message: ";
    std::string string_buffer;
    std::cin.ignore(2, '\n');
    std::getline(std::cin, string_buffer);
    memcpy(buf.message, string_buffer.c_str(), string_buffer.size()+1);
    buf.message[string_buffer.size()] = 0;
    buf.size = strlen(buf.message);
    mq_send(to_server_queue_id, (char*)&buf, sizeof(buf), MSG_2ALL);
}

void read_queue() {
    my_msgbuf_2one_to_server buf;
    unsigned int* type = new unsigned int;
    timespec* timeout = new timespec;
    timeout->tv_sec = 0;
    while(mq_timedreceive(in_queue_id, (char*)&buf, 20000, type, timeout) > 0) {
        switch (*type) {
            case MSG_2ONE: {
                std::cout << "CLIENT: Message received from " << buf.source_id << ": " << buf.message << "\n";
                break;
            }
            case MSG_2ALL: {
                my_msgbuf_2all_to_client buf2 = *(my_msgbuf_2all_to_client *) &buf;
                std::cout << "CLIENT: Message received from " << buf2.source_id << ": " << buf2.message << "\n";
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

int main(){
    struct sigaction sigaction1;
    sigaction1.sa_handler = sigint_handler;
    sigaction(SIGINT, &sigaction1, nullptr);

    to_server_queue_id = mq_open(SERVER_QUEUE_NAME, O_WRONLY);
    if(to_server_queue_id == -1){
        std::cout << "CLIENT: ERROR Failed to get a queue!" << "\n";
        std::cout << "CLIENT: ERROR Reason: " << errno << "\n";
        std::cout << "CLIENT: ERROR Exiting..." << "\n";
        exit(0);
    }
    std::cout << "CLIENT: To-server message queue got successfully. Queue id: " << to_server_queue_id << "\n";

    srand(time(NULL));
    std::string str_buffer = "/client_queue_sysopy" + std::to_string(rand());
    client_queue_name = new char[str_buffer.size() + 1];
    strcpy(client_queue_name, str_buffer.c_str());
    in_queue_id = mq_open(client_queue_name, O_RDONLY | O_CREAT | O_EXCL, 0777, nullptr);
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
    delete client_queue_name;
    return 0;
}