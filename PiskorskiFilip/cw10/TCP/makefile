all: client server

client: client.cpp
	g++ -Wall -o client client.cpp

server: server.cpp
	g++ -Wall -o server server.cpp -std=c++20 -pthread

clean:
	rm -f client server
