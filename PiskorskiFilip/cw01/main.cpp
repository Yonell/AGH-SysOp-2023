#include "fileInfo.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <time.h>

void addFileStats(fileInfo* statistics, std::string fileName){
	statistics -> addFile(fileName);
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
	fileInfo* statistics = nullptr;
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
			statistics = new fileInfo(std::stoi( parameter ));
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
			addFileStats(statistics, parameter);
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
			if(statistics != nullptr){
				delete statistics;
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
		else {
			std::cout << "Invalid option in the line number " << line << ", try again.\n";
		}

		getline(std::cin, instruction);
		line++;
	}
	return 0;
}
