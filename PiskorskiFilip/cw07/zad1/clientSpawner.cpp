#include <iostream>
#include <ctime>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <csignal>
#include <cstdlib>
#define WAITING_ROOM 5
#define BARBER_CHAIRS 3
#define BARBERS 4

int client_id = 0;
int sems_id;
int shm_id;

void sigint_handler(int sig){
    std::cout << "CLIENT SPAWNER: " << " SIGINT signal received! Exiting...\n";
    shm_id = shmget(ftok("/sysopy_cw07", 1), sizeof(int) * (WAITING_ROOM + 2), IPC_CREAT | 0777);
    shmctl(shm_id, IPC_RMID, nullptr);
    semctl(sems_id, 0, IPC_RMID, 0);
    exit(0);
}

void sigint_handler2(int sig){
    std::cout << "CLIENT " << client_id << ": SIGINT signal received! Exiting...\n";
    shm_id = shmget(ftok("/sysopy_cw07", 1), sizeof(int) * (WAITING_ROOM + 2), IPC_CREAT | 0777);
    shmctl(shm_id, IPC_RMID, nullptr);
    semctl(sems_id, 0, IPC_RMID, 0);
    exit(0);
}

void doTheClientThing(){
    struct sigaction sigaction1;
    sigaction1.sa_handler = sigint_handler2;
    sigaction(SIGINT, &sigaction1, nullptr);
    std::cout << "CLIENT " << client_id << ": Spawned. Attempting to sit in a waiting room." << std::endl;
    timespec timespec1;
    timespec1.tv_sec = 0;
    timespec1.tv_nsec = 0;
    sembuf sembuf1;
    sembuf1.sem_num = 0;
    sembuf1.sem_op = -1;
    sembuf1.sem_flg = 0;
    if(semtimedop(sems_id, &sembuf1, 1, &timespec1)){
        std::cout << "CLIENT " << client_id << ": No free waiting spot. Exiting..." << std::endl;
        exit(0);
    }
    key_t key = ftok("/sysopy_cw07", 1);
    shm_id = shmget(key, sizeof(int) * (WAITING_ROOM + 2), IPC_CREAT | 0777);
    if(shm_id == -1){
        std::cout << "CLIENT " << client_id << ": Failed to get shared memory." << std::endl;
        exit(1);
    }
    int *shm = (int*)shmat(shm_id, nullptr, 0);
    if(shm == (int*)-1){
        std::cout << "CLIENT " << client_id << ": Failed to attach shared memory." << std::endl;
        exit(1);
    }
    shm[shm[1]+2] = client_id;
    shm[1] = (shm[1] + 1) % WAITING_ROOM;
    std::cout << "CLIENT " << client_id << ": Sitting in a waiting room." << std::endl;
    while(true){
        if(shm[shm[0]+2] == client_id){
            std::cout << "CLIENT " << client_id << ": I'm first in the queue." << std::endl;
            break;
        }
        sleep(1);
    }
    sembuf1.sem_num = 1;
    sembuf1.sem_op = -1;
    sembuf1.sem_flg = 0;
    semop(sems_id, &sembuf1, 1);
    shm[0] = (shm[0] + 1) % WAITING_ROOM;
    std::cout << "CLIENT " << client_id << ": Sitting in a barber chair." << std::endl;
    sembuf1.sem_num = 2;
    sembuf1.sem_op = -1;
    sembuf1.sem_flg = 0;
    semop(sems_id, &sembuf1, 1);
    sembuf1.sem_num = 0;
    sembuf1.sem_op = 1;
    sembuf1.sem_flg = 0;
    semop(sems_id, &sembuf1, 1);
    std::cout << "CLIENT " << client_id << ": Getting a haircut." << std::endl;
    srand(time(nullptr));
    sleep(rand() % 10 + 1);
    std::cout << "CLIENT " << client_id << ": Leaving the barber shop." << std::endl;
    sembuf1.sem_num = 2;
    sembuf1.sem_op = 1;
    sembuf1.sem_flg = 0;
    semop(sems_id, &sembuf1, 1);
    sembuf1.sem_num = 1;
    sembuf1.sem_op = 1;
    sembuf1.sem_flg = 0;
    semop(sems_id, &sembuf1, 1);
    shmdt(shm);
    exit(0);
}

// semaphors: Waiting room, barber chair, barber
int main(){
    std::cout << "CLIENT SPAWNER: Starting work." << std::endl;
    struct sigaction sigaction1;
    sigaction1.sa_handler = sigint_handler;
    sigaction(SIGINT, &sigaction1, nullptr);
    key_t key = ftok("/sysopy_cw07", 1);
    sems_id = semget(key, 3, IPC_CREAT | 0666);
    if(sems_id == -1){
        std::cout << "CLIENT SPAWNER: Failed to get semaphores." << std::endl;
        return 1;
    }
    union semun {
        int val;
        struct semid_ds *buf;
        unsigned short *array;
        struct seminfo *__buf;
    } semun;
    semun.val = WAITING_ROOM;
    semctl(sems_id, 0, SETVAL, semun);
    semun.val = BARBER_CHAIRS;
    semctl(sems_id, 1, SETVAL, semun);
    semun.val = BARBERS;
    semctl(sems_id, 2, SETVAL, semun);
    while(true){
        if(!fork()){
            doTheClientThing();
        }
        client_id++;
        if(client_id <= 0){
            std::cout << "CLIENT SPAWNER: Too many clients spawned. Exiting..." << std::endl;
            return 1;
        }
        sleep(1);
    }
}