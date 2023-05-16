#include <ncurses.h>
#include <locale.h>
#include <unistd.h>
#include <stdbool.h>
#include "grid.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <pthread.h>
#include <csignal>
#include <iostream>

int threads_count = -1;

struct worker_arg{
    pthread_t thread_id;
    int worker_id;
    char* foreground;
    char* background;
} __attribute__((packed));

void* worker_thread(void* arg){
    struct worker_arg* current_arg = (worker_arg*) arg;
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    int sig;
    while(true){
        sigwait(&set, &sig);
        for(int i = current_arg->worker_id; i < grid_height * grid_width; i += threads_count){
            int row = i / grid_width;
            int col = i % grid_width;
            current_arg->background[i] = is_alive(row, col, current_arg->foreground);
        }
        char* tmp = current_arg->foreground;
        current_arg->foreground = current_arg->background;
        current_arg->background = tmp;
    }
}

void sig_handler(int signo){}

int main(int argc, char *argv[])
{
    if(argc == 2)
    {
        threads_count = atoi(argv[1]);
    }
    else
    {
        threads_count = grid_height * grid_width;
    }
    signal(SIGUSR1, sig_handler);
	srand(time(nullptr));
	setlocale(LC_CTYPE, "");
	initscr(); // Start curses mode

	char *foreground = create_grid();
	char *background = create_grid();
	char *tmp;

	init_grid(foreground);

    std::vector<worker_arg> workers;
    for(int i = 0; i < threads_count; i++)
    {
        auto *worker = new worker_arg();
        worker->worker_id = i;
        worker->foreground = foreground;
        worker->background = background;
        pthread_create(&(worker->thread_id), nullptr, worker_thread, worker);
        workers.push_back(*worker);
    }

	while (true)
	{
		draw_grid(foreground);
        usleep(500 * 1000);

		// Step simulation
        for(int i = 0; i < threads_count; i++)
        {
            pthread_kill(workers[i].thread_id, SIGUSR1);
        }
		tmp = foreground;
		foreground = background;
		background = tmp;
	}

	endwin(); // End curses mode
	destroy_grid(foreground);
	destroy_grid(background);

	return 0;
}
