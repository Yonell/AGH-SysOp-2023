#include <iostream>
#include <ftw.h>
#include <iomanip>
#include <errno.h>

namespace dirCtr{
	long long sum = 0;
	int processDirElement(const char *fpath, const struct stat *sb,
		                 int typeflag){
		if(typeflag == FTW_F){
			std::cout << std::setw(40) << std::left << fpath << std::setw(13) << std::left << sb->st_size << " B\n";
			sum += sb->st_size;
		}
		return 0;
	}
};

int main(int argc, char* argv[]){
	std::cout << std::setw(40) << std::left << "File name" <<  std::setw(40) << std::left << "File size" << "\n";
	int errCode = ftw("./", &dirCtr::processDirElement, 100);
	if(errCode){
		std::cerr << "Error occured!" << "\n";
		std::cerr << "Reason: " << errno << "\n";
	}
	std::cout << std::setw(40) << std::left << "Total:" <<  std::setw(13) << std::left << dirCtr::sum << " B\n";
}
