#include "fileInfo.h"
#include <cstdio>
#include <bits/stdc++.h>
#include <cstdlib>
#include <fstream>
#include <string>
//constructor
fileInfo::fileInfo(){ 
	this -> maxlen = 64;
	this -> len = 0;
}
fileInfo::fileInfo(int maxlen){ 
	this -> maxlen = maxlen;
	this -> len = 0;
}

void fileInfo::addFile(std::string filename){
	if(len == maxlen){
		throw std::runtime_error("Vector overflow");
	}
	int err = system("mkdir -p tmp"); //TODO sprawdzanie, czy plik istnieje
	if(err != 0) throw std::runtime_error("System error: " + err);
	err = system(("cat " + filename + " | wc > ./tmp/tmp.tmp").c_str());
	if(err != 0) throw std::runtime_error("System error: " + err);

	int* temp = (int*) calloc(3, sizeof(int));
	if(temp == nullptr) throw std::runtime_error("Memory allocation error in the file adding function");
	filestats.push_back(temp); //mozna tez stworzyc operatorami new, ale w zadaniu napisane jest by uzyc calloc

	std::ifstream file;
	file.open("./tmp/tmp.tmp");
	if(!file){
		throw std::runtime_error("Failed to load the tmp file.");
	}
	for(int i = 0; i < 3; i++){
		file >> *(filestats[len] + i);
	}
	file.close();

	len++;

	err = system("rm -r ./tmp");
	if(err != 0) throw std::runtime_error("System error: " + err);
}

int* fileInfo::getBlock(int n){
	if(n<0 || n>=(int)filestats.size()){
		std::string errormssg("The index ");
		errormssg += std::to_string(n);
		errormssg += " does not exist (getBlock())";
		throw std::runtime_error(errormssg);
	}
	return filestats[n];
}

void fileInfo::removeBlock(int n){
	if(n<0 || n>=(int)filestats.size()){
		std::string errormssg("The index ");
		errormssg += std::to_string(n);
		errormssg += " does not exist (removeBlock())";
		throw std::runtime_error(errormssg);
	}
	free(filestats[n]);
	filestats.erase(filestats.begin() + n);
	len--;
}

//destructor
fileInfo::~fileInfo(){
	for(int i = 0; i < this -> len; i++){
		free(this -> filestats[i]);
	}
	this -> filestats.clear();
}

/*fileInfo* fileInfoFactoryInner(int maxlen){
	return new fileInfo(maxlen);
}

void fileInfoDeleterInner(void* obj){
	delete ((fileInfo*)obj);
}

void* getFunctionsInner(){
	std::map<std::string, void*>* mapa = new std::map<std::string, void*>();
	mapa -> insert("addFile", &fileInfo::addFile);
	mapa -> insert("getBlock", &fileInfo::getBlock);
	mapa -> insert("removeBlock", &fileInfo::removeBlock);
	return (void*) mapa;
}*/

/*extern "C" {
	void* fileInfoFactory(int maxlen){
		return (void*)fileInfoFactoryInner(maxlen);
	}
	void* getFunctions(){
		return getFunctionsInner();
	}
	void fileInfoDeleter(void* obj){
		fileInfoDeleterInner(obj);
	}
}*/

















