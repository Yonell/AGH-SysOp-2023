#include <iostream>
#include <csignal>
#include <sys/wait.h>

int status;

void write_numbers_from_1_to_100()
{
    status = 1;
    for(int i = 1; i <= 100; i++){
        std::cout << i << "\n";
    }
}

void write_time()
{
    status = 2;
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    std::cout << asctime(timeinfo) << "\n";
}

void write_int(int n)
{
    status = 3;
    std::cout << n << "\n";
}

void write_time_once_per_second(int si_PID)
{
    status = 4;
    kill(si_PID, SIGUSR1);
    while(status == 4){
        write_time();
        sleep(1);
    }
}

void catch_signal(int sig, siginfo_t *info, void *ucontext)
{
    std::cout << "Signal " << sig << " caught" << "\n";
    std::cout << "Sending confirmation" << "\n";
    kill(info->si_pid, SIGUSR1);
    int si_value_int = info->si_value.sival_int;
    static int counter = 0;
    int sender_pid = info->si_pid;
    switch (si_value_int) {
        case 1:
            write_numbers_from_1_to_100();
            break;
        case 2:
            write_time();
            break;
        case 3:
            counter++;
            write_int(counter);
            break;
        case 4:
            write_time_once_per_second(sender_pid);
            break;
        case 5:
            status = 5;
            exit(0);
            break;
        default:
            std::cout << "Unknown value" << "\n";
            break;
    }
}

int main()
{
    std::cout << "Catcher PID: " << getpid() << "\n";

    struct sigaction act;
    act.sa_flags = SA_SIGINFO;
    sigemptyset(&act.sa_mask);
    //sigaddset(&act.sa_mask, SIGUSR1);
    act.sa_sigaction = catch_signal;
    if(sigaction(SIGUSR1, &act, NULL) == -1){
        std::cout << "Error: failed to install the handler" << "\n";
        return 1;
    }
    std::cout << "Handler installed" << "\n";

    while(1){
        int sig;
        std::cout << "Waiting for signal " << SIGUSR1 << "\n";
        sigsuspend(&act.sa_mask);
    }

}