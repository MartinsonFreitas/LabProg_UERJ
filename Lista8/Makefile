CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99

OBJS = main.o bubble.o shell.o quick.o utils.o

TARGET = sort_test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) 

main.o: main.c sort.h utils.h
	$(CC) $(CFLAGS) -c main.c

bubble.o: bubble.c sort.h
	$(CC) $(CFLAGS) -c bubble.c

shell.o: shell.c sort.h
	$(CC) $(CFLAGS) -c shell.c

quick.o: quick.c sort.h
	$(CC) $(CFLAGS) -c quick.c

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c

clean:
	rm -f *.o $(TARGET)
