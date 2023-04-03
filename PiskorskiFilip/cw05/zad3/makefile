all: zad3

zad3: zad3.cpp worker.cpp
	g++ -Wall zad3.cpp -o zad3
	g++ -Wall worker.cpp -o worker

test: zad3
	./zad3 0.00000000001 10
	./zad3 0.00000000001 50
	./zad3 0.00000000001 100
	./zad3 0.00000000001 1000
	
clean:
	rm -f zad3
	rm -f worker
	rm -f cw05zad3pipe
