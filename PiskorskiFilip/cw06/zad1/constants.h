#define COMM_MAX_LENGTH 1000
#define proj_id 255
#define tickrate 2
#define MSG_INIT 1
#define MSG_STOP 2
#define MSG_LIST 3
#define MSG_2ALL 4
#define MSG_2ONE 5

struct my_msgbuf_init_to_server {
    long mtype;
    int type;
    int data;
};
struct my_msgbuf_init_to_client {
    long mtype;
    int type;
    int data;
};

struct my_msgbuf_stop_to_server {
    long mtype;
    int type;
    int data;
};
struct my_msgbuf_stop_to_client {
    long mtype;
    int type;
};

struct my_msgbuf_list_to_server {
    long mtype;
    int type;
    int data;
};
struct my_msgbuf_list_to_client {
    long mtype;
    int type;
    int size;
    int data[COMM_MAX_LENGTH];
};

struct my_msgbuf_2all_to_server {
    long mtype;
    int type;
    int source_id;
    int size;
    char message[COMM_MAX_LENGTH];
};
struct my_msgbuf_2all_to_client {
    long mtype;
    int type;
    int source_id;
    int size;
    char message[COMM_MAX_LENGTH];
};

struct my_msgbuf_2one_to_server {
    long mtype;
    int type;
    int source_id;
    int destination_id;
    int size;
    char message[COMM_MAX_LENGTH];
};
struct my_msgbuf_2one_to_client {
    long mtype;
    int type;
    int source_id;
    int destination_id;
    int size;
    char message[COMM_MAX_LENGTH];
};
