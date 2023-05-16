#include <iostream>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>


pthread_t reindeers[9];
pthread_t elves[10];
pthread_mutex_t santa_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t santa_cond = PTHREAD_COND_INITIALIZER;
bool wake_up_santa = false;
pthread_mutex_t reindeer_mutex = PTHREAD_MUTEX_INITIALIZER;
int reindeers_waiting = 0;
pthread_mutex_t elves_mutex = PTHREAD_MUTEX_INITIALIZER;
int elves_waiting = 0;
int waiting_elves_ids[3];

void sig_handler(int sigint){
    return;
}

void* elves_starting_func(void* arg){
    uint64_t my_id = reinterpret_cast<uint64_t>(arg);
    signal(SIGUSR1, sig_handler);
    while(true){
        sleep(rand()%4+2);
        pthread_mutex_lock(&elves_mutex);
        if(elves_waiting == 3){
            std::cout << "Elf: samodzielnie rozwiązuję swój problem, " << my_id << std::endl;
            pthread_mutex_unlock(&elves_mutex);
            continue;
        }
        elves_waiting++;
        std::cout << "Elf: czeka " << elves_waiting << " elfów na Mikołaja, " << my_id << std::endl;
        waiting_elves_ids[elves_waiting-1] = my_id;
        if(elves_waiting == 3){
            std::cout << "Elf: wybudzam Mikołaja, " << my_id << std::endl;
            wake_up_santa = true;
            pthread_cond_signal(&santa_cond);
        }
        pthread_mutex_unlock(&elves_mutex);
        pause();
        std::cout << "Elf: Mikołaj rozwiązuje problem, " << my_id << std::endl;
    }
}

void* reindeer_starting_func(void* arg){
    uint64_t my_id = reinterpret_cast<uint64_t>(arg);
    signal(SIGUSR1, sig_handler);
    while(true){
        sleep(rand()%6+5);
        pthread_mutex_lock(&reindeer_mutex);
        reindeers_waiting++;
        std::cout << "Renifer: czeka " << reindeers_waiting << " reniferów na Mikołaja, " << my_id << std::endl;
        if(reindeers_waiting == 9){
            std::cout << "Renifer: wybudzam Mikołaja, " << my_id << std::endl;
            wake_up_santa = true;
            pthread_cond_signal(&santa_cond);
        }
        pthread_mutex_unlock(&reindeer_mutex);
        pause();
    }
}

void init_santas_helpers(){
    for(uint64_t i = 0; i < 10; i++){
        if(i < 9){
            pthread_create(&reindeers[i], NULL, reindeer_starting_func, (void*)i);
        }
        pthread_create(&elves[i], NULL, elves_starting_func, (void*)i);
    }
}

int main(int argc, char* argv[]){
    pthread_mutex_lock(&santa_mutex);
    init_santas_helpers();
    srand(time(NULL));

    while(true){
        while (!wake_up_santa) {
            pthread_cond_wait(&santa_cond, &santa_mutex);
        }
        std::cout << "Mikołaj: budzę się" << std::endl;

        while((elves_waiting >= 3) || (reindeers_waiting >= 9)) {

            if (elves_waiting >= 3) {
                std::cout << "Mikołaj: rozwiązuję problemy elfów " << waiting_elves_ids[0] << ", "
                          << waiting_elves_ids[1] << ", " << waiting_elves_ids[2] << std::endl;
                for (int i = 0; i < 3; i++) {
                    sleep(rand() % 2 + 1);
                    pthread_kill(elves[waiting_elves_ids[i]], SIGUSR1);
                    waiting_elves_ids[i] = -1;
                }
                elves_waiting -= 3;
            }

            if (reindeers_waiting >= 9) {
                std::cout << "Mikołaj: dostarczam zabawki" << std::endl;
                reindeers_waiting -= 9;
                sleep(rand() % 6 + 5);
                for (int i = 0; i < 9; i++) {
                    pthread_kill(reindeers[i], SIGUSR1);
                }
            }

        }

        wake_up_santa = false;
        std::cout << "Mikołaj: zasypiam" << std::endl;
    }
}