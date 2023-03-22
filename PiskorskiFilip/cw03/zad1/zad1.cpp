#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

int main( int argc, char* argv[] ){
	int childPID;
	for(int i=0;i<atoi(argv[1]); i++){
		childPID = fork();
		if(childPID == 0){
			std::cout << "PID macierzystego: " << getppid() << " PID dziecka: " << getpid() << "\n";
			break;
		}else{
			wait(NULL);
		}
	}
	if(childPID != 0) std::cout << atoi(argv[1]) << "\n";
	return 0;
}
