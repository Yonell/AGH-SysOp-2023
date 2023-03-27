all: sender catcher

sender: sender.cpp
	g++ sender.cpp -o sender

catcher: catcher.cpp
	g++ catcher.cpp -o catcher

test: all
	./zad3
testcatcher: catcher
	./catcher
testsender: sender
	./sender 12791 1 2 3 4 2 3 1 5

clean:
	rm -f catcher sender
