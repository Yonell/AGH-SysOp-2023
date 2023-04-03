#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]){
    FILE* fortune_pipeline;
    FILE* cowsay_pipeline;
    int totally_read = 0;
    int currently_read;
    std::string mssg_to_the_cow;
    switch(argc){
        case 1:
            fortune_pipeline = popen("fortune","r");
            if(!fortune_pipeline){
                std::cerr << "Failed to open a fortune pipe!!!\n";
            }
            char buffer[2048];
            totally_read = 0;
            currently_read = fread(buffer, sizeof(char), 2048, fortune_pipeline);
            while(currently_read % 2048 != 0){
                totally_read += currently_read;
                std::cout << buffer;
                currently_read = fread(buffer, sizeof(char), 2048, fortune_pipeline);
            }
            pclose(fortune_pipeline);
            break;
        case 4:
            cowsay_pipeline = popen("cowsay", "w");
            if(!cowsay_pipeline){
                std::cerr << "Failed to open a cowsay pipe!!!\n";
            }
            mssg_to_the_cow = "Adres: " + std::string(argv[1]) + "\n"
                    + "Tytul: " + std::string(argv[2]) + "\n"
                    + "Tresc: " + std::string(argv[3]) + "\n";

            fwrite(mssg_to_the_cow.c_str(), mssg_to_the_cow.size(), sizeof(char), cowsay_pipeline);
            pclose(cowsay_pipeline);
            break;
        default:
            std::cout << "Wrong number of arguments!!!\n";
            break;
    }
}