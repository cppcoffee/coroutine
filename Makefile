CC = gcc
CFLAGS = -g -O2 -Wall
RM = rm -f

.PHONY all clean

all: main

main: main.c coroutine.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	$(RM) *.o
	$(RM) main

