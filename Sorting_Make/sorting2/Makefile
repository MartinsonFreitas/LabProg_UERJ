CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99

OBJS = main.o bubble.o shell.o quick.o insertion.o selection.o utils.o

TARGET = sort_test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) 

main.o: main.c sort.h utils.h
	$(CC) $(CFLAGS) -c main.c

bubble.o: bubble.c sort.h utils.h
	$(CC) $(CFLAGS) -c bubble.c

shell.o: shell.c sort.h
	$(CC) $(CFLAGS) -c shell.c

quick.o: quick.c sort.h
	$(CC) $(CFLAGS) -c quick.c

insertion.o: insertion.c sort.h
	$(CC) $(CFLAGS) -c insertion.c

selection.o: selection.c sort.h
	$(CC) $(CFLAGS) -c selection.c

utills.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utills.c

clean:
	rm -f *.o $(TARGET)

