#include <iostream>
#include <signal.h>
#include <sys/wait.h>
#include <chrono>
#include <thread>

#define SIGNAL SIGINT

void handle_signal(int sig, siginfo_t* info, void* ucontext){
	std::cout << "Sig number: " << info->si_signo << "\n";
	std::cout << "PID: " << info->si_pid << "\n";
	std::cout << "User ID: " << info->si_uid << "\n";
	std::cout << "System time: " << info->si_stime << "\n";
	std::cout << "User time: " << info->si_utime << "\n";
}

void child_died_handler(int sig){
	std::cout << "PARENT: Received signal about a childs death!" << "\n";
}

int main(){
	struct sigaction act;
	act.sa_sigaction = &handle_signal;
	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask, SIGNAL);
	act.sa_flags = SA_SIGINFO; 
	std::cout << "Setting up the handler" << "\n";
	if(sigaction(SIGNAL, &act, NULL)){
		std::cerr << "Failed to set up the handler! Exiting..." << "\n";
		exit(0);
	}
	std::cout << "The process will try to kill itself" << "\n";
	raise(SIGNAL);
	std::cout << "The process did not kill itself" << "\n";

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
	std::cout << "\n";
	
	std::cout << "PARENT: Setting up handle without ignoring the childs death" << "\n";
	
	struct sigaction act2;
	act2.sa_handler = &child_died_handler;
	sigemptyset(&act.sa_mask);
	sigaddset(&act2.sa_mask, SIGCHLD);
	act2.sa_flags = 0; 
	
	std::cout << "PARENT: Setting up the handler" << "\n";
	if(sigaction(SIGCHLD, &act2, NULL)){
		std::cerr << "Failed to set up the handler! Exiting..." << "\n";
		exit(0);
	}
	
	std::cout << "PARENT: Making a child..." << "\n";
	if(!fork()){
		std::cout << "CHILD: I'm dying" << "\n";
		exit(0);
	}
	int stat_lock;
	wait(&stat_lock);
	std::cout << "PARENT: Setting up ignoring child death" << "\n";
	
	act2.sa_handler = &child_died_handler;
	sigemptyset(&act.sa_mask);
	//sigaddset(&act2.sa_mask, SIGCHLD);
	act2.sa_flags = SA_NOCLDSTOP; 
	
	std::cout << "PARENT: Setting up the handler" << "\n";
	if(sigaction(SIGCHLD, &act2, NULL)){
		std::cerr << "Failed to set up the handler! Exiting..." << "\n";
		exit(0);
	}
	std::cout << "PARENT: Making a child..." << "\n";
	if(!fork()){
		std::cout << "CHILD: I'm dying" << "\n";
		exit(0);
	}
	std::cout << "PARENT: Now I should not know about the death of my kid." << "\n";

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
	std::cout << "\n";
	
	
	struct sigaction act3;
    act2.sa_handler = &child_died_handler;
	sigemptyset(&act3.sa_mask);
	sigaddset(&act3.sa_mask, SIGCHLD);
	act3.sa_flags = SA_NOCLDWAIT;
	std::cout << "Setting up the handler" << "\n";
	if(sigaction(SIGNAL, &act, NULL)){
		std::cerr << "Failed to set up the handler! Exiting..." << "\n";
		exit(0);
	}
    std::cout << "PARENT: Making a child..." << "\n";
    int child_PID = fork();
    if(!child_PID){
        std::cout << "CHILD: I'm dying " << "\n";
        exit(0);
    }
	std::this_thread::sleep_for(std::chrono::milliseconds(500));

    int err = kill(child_PID, 2);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    if(err == -1){
        std::cout << "PARENT: The child does not exists" << "\n";
    } else {
        std::cout << "PARENT: The child still runs" << "\n";
    }
    int wstatus;
    wait(&wstatus);
	
	return 0;
}
