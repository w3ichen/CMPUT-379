CC = g++
CFLAGS = -c -pthread

all: main.o threads.o tands.o utils.o
	$(CC) -pthread main.o threads.o tands.o utils.o -o prodcon

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp -o main.o

threads.o: threads.cpp
	$(CC) $(CFLAGS) threads.cpp -o threads.o

tands.o: tands.cpp
	$(CC) $(CFLAGS) tands.cpp -o tands.o

utils.o: utils.cpp
	$(CC) $(CFLAGS) utils.cpp -o utils.o
	
optimized: main.o threads.o tands.o utils.o
	$(CC) -pthread main.o threads.o tands.o utils.o -O3 -o prodcon

clean: 
	rm -f *.o *.log prodcon