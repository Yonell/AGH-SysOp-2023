all: zad1fork zad1execl


zad1fork: zad1fork.cpp
	g++ zad1fork.cpp -o zad1fork
	
testzad1fork: zad1fork
	-./zad1fork ignore
	-./zad1fork handler
	-./zad1fork mask
	-./zad1fork pending
	
cleanzad1fork:
	rm -f zad1fork 
	
	
zad1execl: zad1execl.cpp
	g++ zad1execl.cpp -o zad1execl
	
testzad1execl: zad1execl
	-./zad1execl ignore
	-./zad1execl handler
	-./zad1execl mask
	-./zad1execl pending
	
cleanzad1execl:
	rm -f zad1execl


test: testzad1fork testzad1execl

	
clean: cleanzad1fork cleanzad1execl
