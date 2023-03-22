#include <iostream>
#include <unistd.h>

int main( int argc, char* argv[] ){
	if(argc == 1){
		throw std::runtime_error("Too little arguments");
	}
	std::cout << argv[0] << std::flush;
	execl("/bin/ls", "/bin/ls", argv[1], NULL);
}
