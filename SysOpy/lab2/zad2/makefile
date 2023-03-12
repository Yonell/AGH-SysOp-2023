all: zad2

zad2: zad2.cpp
	g++ -Wall zad2.cpp -o zad2
	
test: zad2
	tac < ./loremipsum.txt | rev > ./tmp
	./zad2 ./loremipsum.txt ./output.txt
	diff -s ./tmp ./output.txt
	
clean:
	rm -f zad2
	rm -f output.txt
	rm -f tmp
