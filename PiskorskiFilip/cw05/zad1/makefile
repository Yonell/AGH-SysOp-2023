all: zad1

zad1: zad1.cpp
	g++ -Wall -std=c++20 zad1.cpp -o zad1

test: test1 test2

test1: zad1
	./zad1

test2: zad1
	./zad1 "hehe" "haha" "lubie placki"

clean:
	rm -f ./zad1