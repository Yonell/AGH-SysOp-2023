#include <iostream>
#include <fcntl.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/stat.h>
#include <chrono>

void replace_system_function(char badCharacter, char replaceCharacter, std::string inFileName, std::string outFileName){
	int inFileD = open(inFileName.c_str(), O_RDONLY);
	int outFileD = open(outFileName.c_str(), O_WRONLY | O_CREAT);
	if((inFileD != -1)&&(outFileD != -1)){
		uint8_t buffer;
		while(read(inFileD, &buffer, 1)){
			if((char)buffer != badCharacter)
				write(outFileD, &buffer, 1);
			else
				write(outFileD, &replaceCharacter, 1);
		}
		close(inFileD);
		close(outFileD);
		return;
	}
	std::cout << "Failed to open a file." << "\n";
}

void replace_lib_function(char badCharacter, char replaceCharacter, std::string inFileName, std::string outFileName){
	FILE* inFile = fopen(inFileName.c_str(), "r");
	FILE* outFile = fopen(outFileName.c_str(), "w");
	if((inFile)&&(outFile)){
		uint8_t buffer;
		while(fread(&buffer, 1, 1, inFile)){
			if((char)buffer != badCharacter)
				fwrite(&buffer, 1, 1, outFile);
			else
				fwrite(&replaceCharacter, 1, 1, outFile);
		}
		fclose(inFile);
		fclose(outFile);
		return;
	}
	std::cout << "Failed to open a file." << "\n";
}

int main(int argc, char* argv[]){
	if(argc != 5){
		std::string errormssg("Bad argument number. Expected 4 arguments. Got ");
		errormssg = errormssg + std::to_string(argc-1);
		throw std::runtime_error(errormssg);
	}
	char badCharacter = argv[1][0];
	char replaceCharacter = argv[2][0];
	std::string inFileName(argv[3]);
	std::string outFileName(argv[4]);
	std::cout << "First argument: " << badCharacter << "\n";
	std::cout << "Second argument: " << replaceCharacter << "\n";
	std::cout << "Third argument: " << inFileName << "\n";
	std::cout << "Fourth argument: " << outFileName << "\n";
	std::cout << "\n";
	
	
	
	std::cout << "Replacing by library function 1 byte at a time...\n";
	auto start = std::chrono::steady_clock::now();
	replace_lib_function(badCharacter, replaceCharacter, inFileName, outFileName);
	auto stop = std::chrono::steady_clock::now();
	std::cout << "Done!\n";
	std::cout << "Time elapsed: " << std::chrono::duration_cast<std::chrono::nanoseconds>(stop- start).count() << " ns\n";
	//std::cout << "Press enter key to continue...\n";
	//getchar();
	std::cout << "\n";
	
	std::cout << "Replacing by system function 1 byte at a time...\n";
	start = std::chrono::steady_clock::now();
	replace_system_function(badCharacter, replaceCharacter, inFileName, outFileName);
	stop = std::chrono::steady_clock::now();
	std::cout << "Done!\n";
	std::cout << "Time elapsed: " << std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count() << " ns\n";
	//std::cout << "Press enter key to continue...\n";
	//getchar();
	std::cout << "\n";
	
	return 0;
}
