#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>

#define SIGNAL SIGINT

void test_ignore(){
	std::cout << "PARENT: Setting up ignore..." << "\n";
	signal(SIGNAL, SIG_IGN);
	std::cout << "PARENT: Done..." << "\n";
	std::cout << "PARENT: The parent process will try to kill itself" << "\n";
	raise(SIGNAL);
	std::cout << "PARENT: The parent process ignored the signal" << "\n";
	std::cout << "PARENT: Creating child" << "\n";
	if(!fork()){
		std::cout << "CHILD: Child created!" << "\n";
		std::cout << "CHILD: The child process will try to kill itself" << "\n";
		raise(SIGNAL);
		std::cout << "CHILD: The child process ignored the signal" << "\n";
		exit(0);
	} else {
		int wstatus;
		wait(&wstatus);
	}
	exit(0);
}

void SIGNAL_handler(int signum){
	std::cout << "SIGNAL received!" << "\n";
}

void test_handle(){
	std::cout << "PARENT: Setting up the handler..." << "\n";
	signal(SIGNAL, SIGNAL_handler);
	std::cout << "PARENT: Done..." << "\n";
	std::cout << "PARENT: The parent process will try to kill itself" << "\n";
	raise(SIGNAL);
	std::cout << "PARENT: Creating child" << "\n";
	if(!fork()){
		std::cout << "CHILD: Child created!" << "\n";
		std::cout << "CHILD: The child process will try to kill itself" << "\n";
		raise(SIGNAL);
		exit(0);
	} else {
		int wstatus;
		wait(&wstatus);
	}
	exit(0);
}

void test_mask(){
	sigset_t newmask;
	sigemptyset(&newmask);
	std::cout << "PARENT: Setting up the mask..." << "\n";
	sigaddset(&newmask, SIGNAL);
	if (sigprocmask(SIG_BLOCK, &newmask, NULL) < 0){
		std::cerr << "PARENT: The program did not mask the signal" << "\n";
	} else {
		std::cout << "PARENT: Done..." << "\n";
	}
	std::cout << "PARENT: The parent process will try to kill itself" << "\n";
	raise(SIGNAL);
	std::cout << "PARENT: The parent process masked the signal" << "\n";
	std::cout << "PARENT: Creating child" << "\n";
	if(!fork()){
		std::cout << "CHILD: Child created!" << "\n";
		std::cout << "CHILD: The child process will try to kill itself" << "\n";
		raise(SIGNAL);
		std::cout << "CHILD: The child process masked the signal" << "\n";
		exit(0);
	} else {
		int wstatus;
		wait(&wstatus);
	}
	exit(0);
}

void test_pending(){
	sigset_t newmask;
	sigemptyset(&newmask);
	std::cout << "PARENT: Setting up the mask..." << "\n";
	sigaddset(&newmask, SIGNAL);
	if (sigprocmask(SIG_BLOCK, &newmask, NULL) < 0){
		std::cerr << "PARENT: The program did not mask the signal" << "\n";
	} else {
		std::cout << "PARENT: Done..." << "\n";
	}
	std::cout << "PARENT: The parent process will try to kill itself" << "\n";
	raise(SIGNAL);
	sigset_t set;
	std::cout << "PARENT: Checking the pending signals" << "\n";
	sigpending(&set);
	std::cout << "PARENT: Checking if SIGNAL is pending" << "\n";
	int is_sig_a_member = sigismember(&set, SIGNAL);
	switch(is_sig_a_member){
		case -1:
			std::cerr << "PARENT: An error occured!" << "\n";
			break;
		case 0:
			std::cout << "PARENT: SIGNAL is not pending!" << "\n";
			break;
		case 1:
			std::cout << "PARENT: SIGNAL is pending!" << "\n";
			break;
	}
	std::cout << "PARENT: Creating child" << "\n";
	if(!fork()){
		std::cout << "CHILD: Child created!" << "\n";
		std::cout << "CHILD: Checking the pending signals" << "\n";
		sigpending(&set);
		std::cout << "CHILD: Checking if SIGNAL is pending" << "\n";
		is_sig_a_member = sigismember(&set, SIGNAL);
		switch(is_sig_a_member){
			case -1:
				std::cerr << "CHILD: An error occured!" << "\n";
				break;
			case 0:
				std::cout << "CHILD: SIGNAL is not pending!" << "\n";
				break;
			case 1:
				std::cout << "CHILD: SIGNAL is pending!" << "\n";
				break;
		}
		exit(0);
	} else {
		int wstatus;
		wait(&wstatus);
	}
	exit(0);
}

int main(int argc, char* argv[]){
	if(argc != 2){
		std::cerr << "Bad number of arguments! exiting..." << "\n";
		exit(-1);
	}
	std::string arg(argv[1]);
	if(arg == "ignore"){
		test_ignore();
	} else if(arg == "handler"){
		test_handle();
	} else if(arg == "mask"){
		test_mask();
	} else if(arg == "pending"){
		test_pending();
	} else {
		std::cerr << "Bad argument! exiting..." << "\n";
		exit(-1);
	}
	return 0;
}
