#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>

#define SIGNAL SIGINT

void test_ignore(){
	std::cout << "BEFORE_EXECL: Setting up ignore..." << "\n";
	signal(SIGNAL, SIG_IGN);
	std::cout << "BEFORE_EXECL: Done..." << "\n";
	std::cout << "BEFORE_EXECL: The parent process will try to kill itself" << "\n";
	raise(SIGNAL);
	std::cout << "BEFORE_EXECL: The parent process ignored the signal" << "\n";
	std::cout << "BEFORE_EXECL: Performing execl..." << "\n";
	execl("./zad1execl", "./zad1execl", "ignore_after_execl", NULL);
	std::cerr << "BEFORE_EXECL: execl failed somehow!" << "\n";
}

void SIGNAL_handler(int signum){
	std::cout << "SIGNAL received!" << "\n";
}

void test_handle(){
	std::cout << "BEFORE_EXECL: Setting up the handler..." << "\n";
	signal(SIGNAL, SIGNAL_handler);
	std::cout << "BEFORE_EXECL: Done..." << "\n";
	std::cout << "BEFORE_EXECL: The parent process will try to kill itself" << "\n";
	raise(SIGNAL);
	std::cout << "BEFORE_EXECL: Performing execl..." << "\n";
	execl("./zad1execl", "./zad1execl", "handler_after_execl", NULL);
	std::cerr << "BEFORE_EXECL: execl failed somehow!" << "\n";
}

void test_mask(){
	sigset_t newmask;
	sigemptyset(&newmask);
	std::cout << "BEFORE_EXECL: Setting up the mask..." << "\n";
	sigaddset(&newmask, SIGNAL);
	if (sigprocmask(SIG_BLOCK, &newmask, NULL) < 0){
		std::cerr << "BEFORE_EXECL: The program did not mask the signal" << "\n";
	} else {
		std::cout << "BEFORE_EXECL: Done..." << "\n";
	}
	std::cout << "BEFORE_EXECL: The parent process will try to kill itself" << "\n";
	raise(SIGNAL);
	std::cout << "BEFORE_EXECL: The parent process masked the signal" << "\n";
	std::cout << "BEFORE_EXECL: Performing execl..." << "\n";
	execl("./zad1execl", "./zad1execl", "mask_after_execl", NULL);
	std::cerr << "BEFORE_EXECL: execl failed somehow!" << "\n";
}

void test_pending(){
	sigset_t newmask;
	sigemptyset(&newmask);
	std::cout << "BEFORE_EXECL: Setting up the mask..." << "\n";
	sigaddset(&newmask, SIGNAL);
	if (sigprocmask(SIG_BLOCK, &newmask, NULL) < 0){
		std::cerr << "BEFORE_EXECL: The program did not mask the signal" << "\n";
	} else {
		std::cout << "BEFORE_EXECL: Done..." << "\n";
	}
	std::cout << "BEFORE_EXECL: The parent process will try to kill itself" << "\n";
	raise(SIGNAL);
	sigset_t set;
	std::cout << "BEFORE_EXECL: Checking the pending signals" << "\n";
	sigpending(&set);
	std::cout << "BEFORE_EXECL: Checking if SIGNAL is pending" << "\n";
	int is_sig_a_member = sigismember(&set, SIGNAL);
	switch(is_sig_a_member){
		case -1:
			std::cerr << "BEFORE_EXECL: An error occured!" << "\n";
			break;
		case 0:
			std::cout << "BEFORE_EXECL: SIGNAL is not pending!" << "\n";
			break;
		case 1:
			std::cout << "BEFORE_EXECL: SIGNAL is pending!" << "\n";
			break;
	}
	std::cout << "BEFORE_EXECL: Performing execl..." << "\n";
	execl("./zad1execl", "./zad1execl", "pending_after_execl", NULL);
	std::cerr << "BEFORE_EXECL: execl failed somehow!" << "\n";
}

void test_ignore_after_execl(){
	std::cout << "AFTER_EXECL: Child created!" << "\n";
	std::cout << "AFTER_EXECL: The child process will try to kill itself" << "\n";
	raise(SIGNAL);
	std::cout << "AFTER_EXECL: The child process ignored the signal" << "\n";
	exit(0);
}

void test_handler_after_execl(){
	std::cout << "AFTER_EXECL: Child created!" << "\n";
	std::cout << "AFTER_EXECL: The child process will try to kill itself" << "\n";
	raise(SIGNAL);
	exit(0);
}

void test_mask_after_execl(){
	std::cout << "AFTER_EXECL: Child created!" << "\n";
	std::cout << "AFTER_EXECL: The child process will try to kill itself" << "\n";
	raise(SIGNAL);
	std::cout << "AFTER_EXECL: The child process masked the signal" << "\n";
	exit(0);
}

void test_pending_after_execl(){
	sigset_t set;
	std::cout << "AFTER_EXECL: Child created!" << "\n";
	std::cout << "AFTER_EXECL: Checking the pending signals" << "\n";
	sigpending(&set);
	std::cout << "AFTER_EXECL: Checking if SIGNAL is pending" << "\n";
	int is_sig_a_member = sigismember(&set, SIGNAL);
	switch(is_sig_a_member){
		case -1:
			std::cerr << "AFTER_EXECL: An error occured!" << "\n";
			break;
		case 0:
			std::cout << "AFTER_EXECL: SIGNAL is not pending!" << "\n";
			break;
		case 1:
			std::cout << "AFTER_EXECL: SIGNAL is pending!" << "\n";
			break;
	}
	exit(0);
}

int main(int argc, char* argv[]){
	/*std::cout << "Number of arguments: " << argc << "\n";
	for(int i=0; i<argc; i++){
		std::cout << "Argument " << i << ": " << argv[i] << "\n";
	}*/
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
	} else if(arg == "ignore_after_execl"){
		test_ignore_after_execl();
	} else if(arg == "handler_after_execl"){
		test_handler_after_execl();
	} else if(arg == "mask_after_execl"){
		test_mask_after_execl();
	} else if(arg == "pending_after_execl"){
		test_pending_after_execl();
	} else {
		std::cerr << "Bad argument! exiting..." << "\n";
		exit(-1);
	}
	return 0;
}
