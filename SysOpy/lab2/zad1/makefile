all: zad1

zad1: zad1.cpp
	g++ -Wall zad1.cpp -o zad1
	
zad1Debug: zad1.cpp
	g++ -Wall -g zad1.cpp -o zad1

test: zad1
	./zad1 a @ ./loremipsum.txt ./output.txt ; tr a @ < ./loremipsum.txt > tmp ; diff -s tmp ./output.txt
	
clean:
	rm -f zad1
	rm -f output.txt
	rm -f tmp
