#include <iostream>
#include <chrono>

const int bufferSize = 1024;

int main(int argc, char* argv[]){
	if(argc != 3){
		std::string errmssg;
		errmssg = "Bad number of arguments. Expected 2. Got " + std::to_string(argc-1);
		throw std::runtime_error(errmssg);
	}
	char* buffer;
	
	FILE* inFile = fopen(argv[1], "r");
	FILE* outFile = fopen(argv[2], "w");
	
	
	std::cout << "Reversing the file byte by byte... " << "\n";
	if((inFile)&&(outFile)){
		auto start = std::chrono::steady_clock::now();
		buffer = (char*)malloc(1);
		fseek(inFile,0,SEEK_END);
		while(!fseek(inFile,-2, SEEK_CUR)){
			fread(buffer, 1, 1, inFile);
			fwrite(buffer, 1, 1, outFile);
		}
		free(buffer);
		auto stop = std::chrono::steady_clock::now();
		std::cout << "Done!" << "\n";
		std::cout << "Time elapsed: " << std::chrono::duration_cast<std::chrono::nanoseconds>(stop- start).count() << " ns\n";
		std::cout << "\n";
		
		fclose(inFile);
		fclose(outFile);
	}else{
		std::cout << "Failed to open some files for byte by byte reversing... \n";
		std::cout << "\n";
	}
	
	inFile = fopen(argv[1], "r");
	outFile = fopen(argv[2], "w");
	
	
	std::cout << "Reversing the file in " << bufferSize << "-byte blocks... " << "\n";
	if((inFile)&&(outFile)){
		auto start = std::chrono::steady_clock::now();
		buffer = (char*)malloc(bufferSize);
		char* reversedBuffer = (char*)malloc(bufferSize);
		int totalRead = 0;
		fseek(inFile,0,SEEK_END);
		int fileSize = ftell(inFile) - 1;
		fseek(inFile,bufferSize-1,SEEK_END);
		while(totalRead + bufferSize < fileSize){
			fseek(inFile,-2*bufferSize, SEEK_CUR);
			totalRead += fread(buffer, 1, bufferSize, inFile);
			for(int i=0; i<bufferSize; i++){
				reversedBuffer[i] = buffer[bufferSize-i-1];
			}
			fwrite(reversedBuffer, 1, bufferSize, outFile);
		}
		fseek(inFile,0,SEEK_SET);
		fread(buffer, 1, fileSize-totalRead, inFile);
		for(int i=fileSize-totalRead-1; i>=0; i--){
			reversedBuffer[i] = buffer[fileSize-totalRead-i-1];
		}
		fwrite(reversedBuffer, 1, fileSize-totalRead, outFile);
		free(buffer);
		free(reversedBuffer);
		auto stop = std::chrono::steady_clock::now();
		std::cout << "Done!" << "\n";
		std::cout << "Time elapsed: " << std::chrono::duration_cast<std::chrono::nanoseconds>(stop- start).count() << " ns\n";
		std::cout << "\n";
		buffer[0] = (char) 0x0a;
		fwrite(buffer, 1, 1, outFile);
		
		fclose(inFile);
		fclose(outFile);
	}else{
		std::cout << "Failed to open some files for byte by byte reversing... \n";
		std::cout << "\n";
	}
}
