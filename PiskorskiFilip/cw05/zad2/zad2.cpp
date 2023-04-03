#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <wait.h>
#include <chrono>

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
    int n = atoi(argv[2]);
    int fd[n][2];
    auto begin = std::chrono::high_resolution_clock::now();
    for(int i=0; i<n; i++){
        int err = pipe(fd[i]);
        if(err){
            std::cout << "Failed to open a pipe... Exiting!\n";
            exit(0);
        }
        if(!fork()){ // dziecko
            close(fd[i][0]);
            double result = calculate_result(i, n, std::stod(argv[1]));
            write(fd[i][1], (void*)(&result), sizeof(result));
            close(fd[i][1]);
            exit(0);
        } else { // rodzic
            close(fd[i][1]);
        }
    }
    for(int j = 0; j < n; j++){
        wait(nullptr);
    }
    double sum = 0;
    for(int j = 0; j < n; j++){
        double buffer;
        read(fd[j][0], (void*)(&buffer), sizeof(double));
        sum += buffer;
        close(fd[j][0]);
    }
    std::cout << "Sum: " << sum << "\n";
    std::cout << "Time elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - begin).count() << "ms" << "\n";
    return 0;
}