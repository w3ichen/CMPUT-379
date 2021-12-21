CC=g++
CFLAGS = -c

all: main.o commands.o userInput.o utils.o
	$(CC) main.o commands.o userInput.o utils.o -o shell379

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp -o main.o

commands.o: commands.cpp
	$(CC) $(CFLAGS) commands.cpp -o commands.o

userInput.o: userInput.cpp
	$(CC) $(CFLAGS) userInput.cpp -o userInput.o

utils.o: utils.cpp
	$(CC) $(CFLAGS) utils.cpp -o utils.o

clean: 
	rm -f *.o shell379