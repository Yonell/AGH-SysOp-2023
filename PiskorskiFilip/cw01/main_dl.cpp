#include "fileInfo.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <time.h>
#include <dlfcn.h>

//It doesn't work because of the name mangling in c++ which is hard to overcome

void addFileStats(void* handle, fileInfo* statistics, std::string fileName){
	//void (*func)(std::string) = (void (*)(std::string))dlsym(handle,"fileInfo::addFile"); //It does not work either
	std::map<std::string, void*>* getFunctions;
	getFunctions = (*((std::map<std::string, void*>* (*)())dlsym(handle,"getFunctions")))();
	statistics -> (*(getFunctions->at("addFile")))(fileName);
}

void showFileStats(fileInfo* statistics, int n){
	int* result = statistics -> getBlock(n);
	std::cout << "Lines: " << result[0] << "\n"
		<< "Words: " << result[1] << "\n"
		<< "Characters: " << result[2] << "\n";
}
void deleteFileStats(fileInfo* statistics, int n){
	statistics -> removeBlock(n);
}

int main(){
    	void *handle = dlopen("libfileInfo.so", RTLD_LAZY);
    	if(!handle){
    		throw std::runtime_error("Failed to load the library");
    	}
	fileInfo* statistics;
	std::string instruction;
	getline(std::cin, instruction);
	timespec *tsrealstart = (timespec*) malloc(sizeof(timespec));
	timespec *tsuserstart = (timespec*) malloc(sizeof(timespec));
	timespec *tssystemstart = (timespec*) malloc(sizeof(timespec));
	timespec *tsrealend = (timespec*) malloc(sizeof(timespec));
	timespec *tsuserend = (timespec*) malloc(sizeof(timespec));
	timespec *tssystemend = (timespec*) malloc(sizeof(timespec));
	
	int line = 1;
	while(instruction != "exit"){
		std::istringstream commandStream(instruction);
		std::string command;
		commandStream >> command;
		
		if(command == "init"){
			std::cout << "Initializing the fileStats..." << "\n";
			std::string parameter;
			commandStream >> parameter;
			clock_gettime(CLOCK_REALTIME, tsrealstart);
			clock_gettime(CLOCK_REALTIME, tsuserstart);
			clock_gettime(CLOCK_REALTIME, tssystemstart);
			fileInfo* (*fileInfoFactory)(int);
			fileInfoFactory = (fileInfo* (*)(int))dlsym(handle, "fileInfoFactory");
			if(dlerror()!=NULL){
				throw std::runtime_error("Failed to load the function");
			}
			statistics = (*fileInfoFactory)(std::stoi( parameter ));
			clock_gettime(CLOCK_REALTIME, tsrealend);
			clock_gettime(CLOCK_REALTIME, tsuserend);
			clock_gettime(CLOCK_REALTIME, tssystemend);
			std::cout << "fileStats initilized!" << "\n";
			std::cout << "Operation duration:\n";
			std::cout << "Real: " << tsrealend->tv_nsec - tsrealstart->tv_nsec << "ns\n";
			std::cout << "User: " << tsuserend->tv_nsec - tsuserstart->tv_nsec << "ns\n";
			std::cout << "Real: " << tssystemend->tv_nsec - tssystemstart->tv_nsec << "ns\n";
			std::cout << "\n";
		}
		else if(command == "count"){
			std::string parameter;
			commandStream >> parameter;
			std::cout << "Adding file " << parameter << " to the fileStats..." << "\n";
			clock_gettime(CLOCK_REALTIME, tsrealstart);
			clock_gettime(CLOCK_REALTIME, tsuserstart);
			clock_gettime(CLOCK_REALTIME, tssystemstart);
			try{
				addFileStats(handle, statistics, parameter);
			}catch( const std::exception & ex ) {
				std::cout << "Failed to add the file!\nReason: " << ex.what() << "\n";
			}
			clock_gettime(CLOCK_REALTIME, tsrealend);
			clock_gettime(CLOCK_REALTIME, tsuserend);
			clock_gettime(CLOCK_REALTIME, tssystemend);
			std::cout << "Done" << "\n";
			std::cout << "Operation duration:\n";
			std::cout << "Real: " << tsrealend->tv_nsec - tsrealstart->tv_nsec << "ns\n";
			std::cout << "User: " << tsuserend->tv_nsec - tsuserstart->tv_nsec << "ns\n";
			std::cout << "Real: " << tssystemend->tv_nsec - tssystemstart->tv_nsec << "ns\n";
			std::cout << "\n";
		}
		else if(command == "show"){
			std::string parameter;
			commandStream >> parameter;
			std::cout << "Showing fileStats on index " << parameter << "..." << "\n";
			clock_gettime(CLOCK_REALTIME, tsrealstart);
			clock_gettime(CLOCK_REALTIME, tsuserstart);
			clock_gettime(CLOCK_REALTIME, tssystemstart);
			try{
			showFileStats(statistics, std::stoi( parameter ));
			}catch( const std::exception & ex ) {
				std::cout << "Failed to show the stats!\nReason: " << ex.what() << "\n";
			}
			clock_gettime(CLOCK_REALTIME, tsrealend);
			clock_gettime(CLOCK_REALTIME, tsuserend);
			clock_gettime(CLOCK_REALTIME, tssystemend);
			std::cout << "Operation duration:\n";
			std::cout << "Real: " << tsrealend->tv_nsec - tsrealstart->tv_nsec << "ns\n";
			std::cout << "User: " << tsuserend->tv_nsec - tsuserstart->tv_nsec << "ns\n";
			std::cout << "Real: " << tssystemend->tv_nsec - tssystemstart->tv_nsec << "ns\n";
			std::cout << "\n";
		}
		else if(command == "delete"){
			commandStream >> command;
			if(command == "index"){
				std::string parameter;
				commandStream >> parameter;
				std::cout << "Deleting fileStats on index " << parameter << "..." << "\n";
				clock_gettime(CLOCK_REALTIME, tsrealstart);
				clock_gettime(CLOCK_REALTIME, tsuserstart);
				clock_gettime(CLOCK_REALTIME, tssystemstart);
				try{
				deleteFileStats(statistics, std::stoi( parameter ));
				}catch( const std::exception & ex ) {
					std::cout << "Failed to delete the stats!\nReason: " << ex.what() << "\n";
				}
				clock_gettime(CLOCK_REALTIME, tsrealend);
				clock_gettime(CLOCK_REALTIME, tsuserend);
				clock_gettime(CLOCK_REALTIME, tssystemend);
				std::cout << "Done" << "\n";
				std::cout << "Operation duration:\n";
				std::cout << "Real: " << tsrealend->tv_nsec - tsrealstart->tv_nsec << "ns\n";
				std::cout << "User: " << tsuserend->tv_nsec - tsuserstart->tv_nsec << "ns\n";
				std::cout << "Real: " << tssystemend->tv_nsec - tssystemstart->tv_nsec << "ns\n";
				std::cout << "\n";
			} else {
				std::cout << "Invalid option in the line number " << line << ", try again.\n";
			}
		}
		else if(command == "destroy"){
			std::cout << "Destroying the fileStats object... " << "\n";
			clock_gettime(CLOCK_REALTIME, tsrealstart);
			clock_gettime(CLOCK_REALTIME, tsuserstart);
			clock_gettime(CLOCK_REALTIME, tssystemstart);
			delete statistics;
			clock_gettime(CLOCK_REALTIME, tsrealend);
			clock_gettime(CLOCK_REALTIME, tsuserend);
			clock_gettime(CLOCK_REALTIME, tssystemend);
			std::cout << "Done" << "\n";
			std::cout << "Operation duration:\n";
			std::cout << "Real: " << tsrealend->tv_nsec - tsrealstart->tv_nsec << "ns\n";
			std::cout << "User: " << tsuserend->tv_nsec - tsuserstart->tv_nsec << "ns\n";
			std::cout << "Real: " << tssystemend->tv_nsec - tssystemstart->tv_nsec << "ns\n";
			std::cout << "\n";
		}
		else {
			std::cout << "Invalid option in the line number " << line << ", try again.\n";
		}

		getline(std::cin, instruction);
		line++;
	}
	dlclose(handle);
	return 0;
}
