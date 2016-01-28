CC = gcc
SOURCES = $(wildcard src/*.c)
OBJS = $(SOURCES:.c=.o)
COPTS = -lreadline -Wall -Wextra

bshell: $(OBJS)
	$(CC) $(COPTS) $(OBJS) -o bshell

.PHONY: clean
clean:
	rm -f bshell $(OBJS)
