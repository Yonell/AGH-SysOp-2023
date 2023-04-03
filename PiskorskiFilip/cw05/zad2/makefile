all: zad2

zad2: zad2.cpp
	g++ -Wall zad2.cpp -o zad2
	
test: zad2
	./zad2 0.0000000001 10
	./zad2 0.0000000001 50
	./zad2 0.0000000001 100
	./zad2 0.0000000001 1000
	
clean:
	rm -f zad2
