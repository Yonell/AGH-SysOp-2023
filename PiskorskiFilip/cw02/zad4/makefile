all: zad4

zad4: zad4.cpp
	g++ -Wall zad4.cpp -o zad4
	
test: zad4
	./zad4
	find ./ | xargs wc --bytes
	
clean:
	rm -f zad4
