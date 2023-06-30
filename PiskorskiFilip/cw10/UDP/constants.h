#define COMM_MAX_LENGTH 2000
#define proj_id 255
#define tickrate 2
#define MSG_STOP 1
#define MSG_INIT 2
#define MSG_LIST 3
#define MSG_2ALL 4
#define MSG_2ONE 5
#define MSG_PING 6

struct my_msgbuf_init_to_server {
    int type;
    char data[200];
} __attribute__((packed));

struct my_msgbuf_init_to_client {
    int type;
    int data;
} __attribute__((packed));

struct my_msgbuf_stop_to_server {
    int type;
    int data;
} __attribute__((packed));
struct my_msgbuf_stop_to_client {
    int type;
} __attribute__((packed));

struct my_msgbuf_list_to_server {
    int type;
    int data;
} __attribute__((packed));
struct my_msgbuf_list_to_client {
    int type;
    int size;
    int data[COMM_MAX_LENGTH];
} __attribute__((packed));

struct my_msgbuf_2all_to_server {
    int type;
    int source_id;
    int size;
    char message[COMM_MAX_LENGTH];
} __attribute__((packed));
struct my_msgbuf_2all_to_client {
    int type;
    int source_id;
    int size;
    char message[COMM_MAX_LENGTH];
} __attribute__((packed));

struct my_msgbuf_2one_to_server {
    int type;
    int source_id;
    int destination_id;
    int size;
    char message[COMM_MAX_LENGTH];
} __attribute__((packed));
struct my_msgbuf_2one_to_client {
    int type;
    int source_id;
    int destination_id;
    int size;
    char message[COMM_MAX_LENGTH];
} __attribute__((packed));
