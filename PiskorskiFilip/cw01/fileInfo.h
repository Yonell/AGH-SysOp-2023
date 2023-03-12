#include <cstdio>
#include <bits/stdc++.h>
#include <cstdlib>
#include <fstream>

class fileInfo {
	std::vector<int*> filestats;
	int maxlen;
	int len; //teoretycznie jest tez ta informacja w vector.size(), ale specjalnie dla opisu zadania dodam tez to pole
	
	public:
	
		fileInfo();
	
		fileInfo(int maxLen);
	
		void addFile(std::string filename);

		int* getBlock(int n);

		void removeBlock(int n);
		
		~fileInfo();
};
