#include <iostream>
#include <csignal>
#include <chrono>

void sigusr1_handler(int num){
    std::cout << "Signal " << num << " received" << "\n";
}

int main(int argc, char *argv[])
{
    struct sigaction sa;
    sa.sa_handler = sigusr1_handler;
    sigemptyset(&sa.sa_mask);
    if(sigaction(SIGUSR1, &sa, NULL) == -1){
        std::cout << "Error: failed to install the handler" << "\n";
        return 1;
    }
    int pid = atoi(argv[1]);
    for(int i = 2; i < argc; i++){
        int value = atoi(argv[i]);
        union sigval sv_value;
        sv_value.sival_int = value;
        sigqueue(pid, SIGUSR1, sv_value);
        std::cout << "Signal " << SIGUSR1 << ", value: " << value << " sent to PID: " << pid << "\n";
        std::cout << "Waiting for signal " << SIGUSR1 << "\n";
        sigset_t mask;
        sigemptyset(&mask);
        sigsuspend(&mask);
    }
    return 0;
}