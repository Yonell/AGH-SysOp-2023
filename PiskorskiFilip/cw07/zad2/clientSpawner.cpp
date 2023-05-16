#include <iostream>
#include <ctime>
#include <unistd.h>
#include <sys/types.h>
#include <csignal>
#include <cstdlib>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#define WAITING_ROOM 5
#define BARBER_CHAIRS 3
#define BARBERS 4

int client_id = 0;
sem_t* sem_wr;
sem_t* sem_bc;
sem_t* sem_b;
int shm_id;
int* shm;

void sigint_handler(int sig){
    std::cout << "CLIENT SPAWNER: " << " SIGINT signal received! Exiting...\n";
    shm_unlink("/sysopy_cw07");
    sem_close(sem_wr);
    sem_close(sem_bc);
    sem_close(sem_b);
    sem_unlink("/sysopy_cw07_wr");
    sem_unlink("/sysopy_cw07_bc");
    sem_unlink("/sysopy_cw07_b");
    exit(0);
}

void sigint_handler2(int sig){
    std::cout << "CLIENT " << client_id << ": SIGINT signal received! Exiting...\n";
    munmap(shm, sizeof(int) * (WAITING_ROOM + 2));
    sem_close(sem_wr);
    sem_close(sem_bc);
    sem_close(sem_b);
    sem_unlink("/sysopy_cw07_wr");
    sem_unlink("/sysopy_cw07_bc");
    sem_unlink("/sysopy_cw07_b");
    exit(0);
}

void doTheClientThing(){
    struct sigaction sigaction1;
    sigaction1.sa_handler = sigint_handler2;
    sigaction(SIGINT, &sigaction1, nullptr);
    std::cout << "CLIENT " << client_id << ": Spawned. Attempting to sit in a waiting room." << std::endl;
    if(sem_trywait(sem_wr) == -1){
        std::cout << "CLIENT " << client_id << ": No free waiting spot. Exiting..." << std::endl;
        exit(0);
    }
    shm_id = shm_open("/sysopy_cw07", O_RDWR, 0777);
    if(shm_id == -1){
        std::cout << "CLIENT " << client_id << ": Failed to get shared memory." << std::endl;
        exit(1);
    }
    if(ftruncate(shm_id, sizeof(int) * (WAITING_ROOM + 2)) == -1){
        std::cout << "CLIENT " << client_id << ": Failed to resize shared memory." << std::endl;
        exit(1);
    }
    shm = (int*)mmap(nullptr, sizeof(int) * (WAITING_ROOM + 2), PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0);
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
    sem_wait(sem_bc);
    shm[0] = (shm[0] + 1) % WAITING_ROOM;
    std::cout << "CLIENT " << client_id << ": Sitting in a barber chair." << std::endl;
    sem_wait(sem_b);
    sem_post(sem_wr);
    std::cout << "CLIENT " << client_id << ": Getting a haircut." << std::endl;
    srand(time(nullptr));
    sleep(rand() % 10 + 1);
    std::cout << "CLIENT " << client_id << ": Leaving the barber shop." << std::endl;
    sem_post(sem_b);
    sem_post(sem_bc);
    munmap(shm, sizeof(int) * (WAITING_ROOM + 2));
    exit(0);
}

// semaphors: Waiting room, barber chair, barber
int main(){
    std::cout << "CLIENT SPAWNER: Starting work." << std::endl;
    struct sigaction sigaction1;
    sigaction1.sa_handler = sigint_handler;
    sigaction(SIGINT, &sigaction1, nullptr);
    key_t key = ftok("/sysopy_cw07", 1);
    sem_wr = sem_open("/sysopy_cw07_wr", O_CREAT, 0777, WAITING_ROOM);
    sem_bc = sem_open("/sysopy_cw07_bc", O_CREAT, 0777, BARBER_CHAIRS);
    sem_b = sem_open("/sysopy_cw07_b", O_CREAT, 0777, BARBERS);
    if(sem_wr == SEM_FAILED || sem_bc == SEM_FAILED || sem_b == SEM_FAILED){
        std::cout << "CLIENT SPAWNER: Failed to get semaphores." << std::endl;
        return 1;
    }
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