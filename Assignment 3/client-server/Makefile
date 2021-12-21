CC = g++
CFLAGS = -c

all: client.o server.o tands.o utils.o
	$(CC) client.o utils.o tands.o -O -o client
	$(CC) server.o utils.o tands.o -O -o server

client.o: client.cpp
	$(CC) $(CFLAGS) client.cpp -o client.o

server.o: server.cpp
	$(CC) $(CFLAGS) server.cpp -o server.o

tands.o: tands.cpp
	$(CC) $(CFLAGS) tands.cpp -o tands.o

utils.o: utils.cpp
	$(CC) $(CFLAGS) utils.cpp -o utils.o

man:
	groff -man -Tpdf ./server.man > server.pdf
	groff -man -Tpdf ./client.man > client.pdf

clean: 
	rm -f *.o *.log server client