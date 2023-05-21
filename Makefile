CC = gcc
CFLAGS = -Wall -Wextra -pedantic

TARGET = react_server

.PHONY: all clean

all: $(TARGET)

$(TARGET): reactor.o
	$(CC) $(CFLAGS) -o $(TARGET) reactor.o

reactor.o: reactor.c
	$(CC) $(CFLAGS) -c reactor.c

clean:
	rm -f $(TARGET) *.o
