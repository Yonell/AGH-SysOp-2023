all: zad3

zad3: zad3.cpp
	g++ -std=c++23 -Wall zad3.cpp -o zad3
	
test: zad3
	./zad3 ./ \#include
	./zad3 . \#include
	./zad3 .. \#include
	./zad3 ../.. \#include
	./zad3 ~ \#include
	./zad3 /root \#include
	./zad3 ./ asdf
	./zad3 . asdf
	./zad3 .. asdf
	./zad3 ../.. asdf
	./zad3 ~ asdf
	./zad3 /root asdf
	./zad3 ./ zadanie
	./zad3 . zadanie
	./zad3 .. zadanie
	./zad3 ../.. zadanie
	./zad3 ~ zadanie
	./zad3 /root zadanie
	
clean:
	rm -f zad3
