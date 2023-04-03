#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <wait.h>
#include <chrono>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

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
    auto begin = std::chrono::high_resolution_clock::now();
    mode_t mode = 0777;
    int err = mkfifo("cw05zad3pipe", mode);
    if(!err){
        //std::cerr << "Failed to create a pipe!!! Errno: " << errno << "\n";
        exit(0);
    }
    for(int i=0; i<n; i++) {
        //std::cerr << "MAIN PROGRAM: Starting " << i << "\'th kid" << "\n";
        if (!fork()) { // dziecko
            execl("./worker", "worker", std::to_string(i).c_str(), argv[2], argv[1], NULL);
        }
    }
    //std::cerr << "MAIN PROGRAM: Waiting for the children to finish their work" << "\n";
    int pipe_in = open("./cw05zad3pipe", O_NONBLOCK | O_RDONLY);
    for(int j = 0; j < n; j++){
        wait(nullptr);
    }
    //std::cerr << "MAIN PROGRAM: All the children have died" << "\n";
    double sum = 0;
    for(int j = 0; j < n; j++){
        double buffer;
        read(pipe_in, (void*)(&buffer), sizeof(double));
        sum += buffer;
    }
    std::cout << "Sum: " << sum << "\n";
    std::cout << "Time elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - begin).count() << "ms" << "\n";
    close(pipe_in);
    return 0;
}
