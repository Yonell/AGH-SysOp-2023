#include <iostream>
#include <iomanip>
#include <dirent.h>
#include <sys/stat.h>
#include <string>
#include <errno.h>

int main(){
	DIR* currentDir = opendir("./");
	if(currentDir == nullptr) throw std::runtime_error("The program is not able to open a catalog");
	std::cout << std::setw(40) << std::left << "File name" <<  std::setw(40) << std::left << "File size" << "\n";
	struct dirent* dirElement = readdir(currentDir);
	dirElement = readdir(currentDir);
	long long sum = 0;
	struct stat* statBuf = new struct stat;
	while(dirElement != nullptr){
		std::string fileName(dirElement->d_name);
		fileName = "./" + fileName;
		int isErr = stat(fileName.c_str(), statBuf);
		if(isErr){
			std::cerr << "Failed to read the file: " << fileName << "\n";
			std::cerr << "Cause: " << errno << "\n";
		}
		if(!S_ISDIR(statBuf->st_mode)){
			std::cout << std::setw(40) << std::left << fileName << std::setw(13) << std::left << statBuf->st_size << " B\n";
			sum += statBuf->st_size;
		}
		
		dirElement = readdir(currentDir);
	}
	std::cout << std::setw(40) << std::left << "Total:" <<  std::setw(13) << std::left << sum << " B\n";
	closedir(currentDir);
}
