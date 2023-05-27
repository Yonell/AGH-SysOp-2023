CC = gcc
CFLAGS = -Wall

all: test
	
zad1: zad1.c zad1.h
	$(CC) $(CFLAGS) zad1.c -o zad1

zad2: zad2.c zad2.h check_fork.c
	$(CC) $(CFLAGS) check_fork.c -o check_fork
	$(CC) $(CFLAGS) zad2.c -o zad2

zad3: zad3.c zad3.h
	$(CC) $(CFLAGS) zad3.c -o zad3

test: zad1 zad2 zad3
	./zad1 | tee results.txt
	./zad2 | tee -a results.txt
	./zad3 | tee -a results.txt
	
clean:
	rm -f zad1 zad2 zad3 check_fork
