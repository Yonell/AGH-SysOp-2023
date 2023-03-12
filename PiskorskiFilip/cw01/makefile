CC=g++
CFLAGS=-c -Wall
OFLAGS=-O3

all: programStaticLibrary programSharedLibrary


programStaticLibrary: programStaticObjectFile staticLibrary
	$(CC) -Wall $(OFLAGS) ./bin/main.o -L./bin/static -lfileInfo -o ./bin/statically-linked-executable

programStaticObjectFile: main.cpp
	mkdir -p bin
	$(CC) $(CFLAGS) $(OFLAGS) main.cpp -o ./bin/main.o

cleanStaticProgram:
	rm -f ./bin/statically-linked-executable
	rm -f ./bin/main.o


programSharedLibrary: programSharedObjectFile sharedLibrary
	$(CC) -Wall $(OFLAGS) ./bin/main.o -L./lib/shared -lfileInfo -o ./bin/shared-library-executable

programSharedObjectFile: main.cpp
	mkdir -p bin
	$(CC) $(CFLAGS) $(OFLAGS) main.cpp -o ./bin/main.o

cleanSharedProgram:
	rm -f ./bin/shared-library-executable
	rm -f ./bin/main.o


#legacy (it doesnt work!)
#programDynamicallyLoaded: programDynamicallyLoadedObjectFile sharedLibrary
#	$(CC) -Wall $(OFLAGS) ./bin/main_dl.o -L./lib/shared -lfileInfo -o ./bin/dynamically-loaded-library-executable
#
#programDynamicallyLoadedObjectFile: main_dl.cpp
#	mkdir -p bin
#	$(CC) $(CFLAGS) $(OFLAGS) main_dl.cpp -o ./bin/main_dl.o
#
#cleanDynamicallyLoadedProgram:
#	rm -f ./bin/dynamically-loaded-library-executable
#	rm -f ./bin/main_dl.o


staticLibrary: staticObjectFile
	mkdir -p bin/static
	ar rcs ./bin/static/libfileInfo.a ./bin/fileInfo.o

staticObjectFile: fileInfo.cpp
	mkdir -p bin
	$(CC) $(CFLAGS) $(OFLAGS) fileInfo.cpp -o ./bin/fileInfo.o
	
cleanStatic:
	rm -f -r ./bin/static
	rm -f ./bin/fileInfo.o
	rm -f *.a
	

sharedLibrary: sharedObjectFile
	mkdir -p bin/shared/
	$(CC) $(OFLAGS) -shared ./bin/fileInfo.o -o ./bin/shared/libfileInfo.so
	sudo cp ./bin/shared/libfileInfo.so /usr/lib/

sharedObjectFile: fileInfo.cpp
	mkdir -p bin
	$(CC) $(CFLAGS) $(OFLAGS) -fPIC fileInfo.cpp -o ./bin/fileInfo.o
	
cleanShared:
	rm -f -r ./bin/shared/
	rm -f ./bin/fileInfo.o
	rm -f *.so
	sudo rm -f /usr/lib/libfileInfo.so
	
testStaticLib: programStaticLibrary
	./bin/statically-linked-executable < ./testCommands.txt > ./results/results_static_$(OFLAGS).txt
	
testSharedLib: programSharedLibrary
	./bin/shared-library-executable < ./testCommands.txt > ./results/results_shared_$(OFLAGS).txt
	
test: testStaticLib testSharedLib
	
	
clean: cleanShared cleanStatic cleanStaticProgram cleanSharedProgram
	rm -f -r ./bin/
