#include <iostream>
#include <unistd.h>
#include <cstdlib>

inline double func(double x){
    return 4/(x*x + 1);
}

double calculate_result(int i, int n, double width){
    double sum = 0;
    for(double j = i*width; j < 1; j += n*width){
        if(j+width < 1) {
            sum += func(j) * (width);
        }else{
            sum += func(j) * (1-j);
        }
    }
    return sum;
}

int main(int argc, char* argv[]){
    if(argc != 4){
        //std::cerr << "Failed to launch the program!!!" << "\n";
        exit(0);
    }
    int i = atoi(argv[1]);
    //std::cerr << "Sub-program " << i << " launched" << "\n";
    int n = atoi(argv[2]);
    double width = atof(argv[3]);
    //std::cerr << "i = " << i << "\n";
    //std::cerr << "n = " << n << "\n";
    //std::cerr << "width = " << width << "\n";

    //std::cerr << "Calculating..." << "\n";
    double result = calculate_result(i, n, width);
    //std::cerr << "Calculations finished" << "\n";
    FILE* file;
    file = fopen("cw05zad3pipe", "w");
    if(file == nullptr){
        //std::cerr << "Failed to open a pipe for writing!" << "\n";
    }
    //std::cerr << "Writing to a pipe..." << "\n";
    fwrite((void*)(&result), sizeof(double), 1, file);
    //std::cerr << "Finished writing to a pipe, closing and exiting..." << "\n";
    fclose(file);
    //std::cerr << "Program " << i << " finished" << "\n";
    exit(0);
}