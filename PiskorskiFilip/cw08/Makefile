CC = g++
CFLAGS = -Wall

all: life
	
life: grid.cpp life.cpp
	$(CC) $(CFLAGS) grid.cpp life.cpp -o life -lncursesw
	
clean:
	rm -f life
