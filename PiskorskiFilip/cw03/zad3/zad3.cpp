#include <iostream>
#include <fstream>
#include <filesystem>
#include <stdio.h>
#include <unistd.h>

void iterate_dir(std::filesystem::path dir_path, std::string check_arg){
	std::filesystem::directory_iterator dir_iter(dir_path);
	for(auto& dir_elem : dir_iter){
		if(dir_elem.is_directory()){ 
			std::string path(dir_elem.path());
			if((path[path.length()-2]=='/' && path[path.length()-1]=='.') || (path[path.length()-3]=='/' && path[path.length()-2]=='.' && path[path.length()-1]=='.')){
				continue;
			}
			
			pid_t child_PID = fork();
			if(child_PID == 0){
				iterate_dir(std::filesystem::path(dir_elem.path()), check_arg);
				return;
			} else {
				continue;
			}
		} else {
			std::ifstream in_file;
			in_file.open(dir_elem.path().string());
			
			char buffer[check_arg.length()+1];
			buffer[check_arg.length()] = 0;
			in_file.read(buffer, check_arg.length());
			std::string from_file(buffer);
			
			if(in_file.gcount() == (long int) check_arg.length()){
				if(from_file == check_arg){
					std::cout << "File path: " << dir_elem.path() << " ; PID procesu: " << getpid() << ";\n";
				}
			}
		
			in_file.close();
		}
	}
	return;
}

int main( int argc, char* argv[] ) {
	if(argc != 3){
		std::string errmssg("Wrong number of arguments! Got ");
		errmssg += std::to_string(argc);
		errmssg += ", expected ";
		errmssg += "3";
		throw std::runtime_error(errmssg);
	}
	try{
		iterate_dir(std::filesystem::path(argv[1]), std::string(argv[2]));
	}catch(std::exception& e){
		std::cerr << "Some error occured! Reason: " << e.what() << "\n";
	}
}
