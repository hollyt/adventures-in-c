CC = gcc
SOURCES = $(wildcard src/*.c)
OBJS = $(SOURCES:.c=.o)

bshell: $(OBJS)
	$(CC) $(OBJS) -o bshell

.PHONY: clean
clean:
	rm -f bshell $(OBJS)
