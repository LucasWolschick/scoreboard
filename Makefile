CC = gcc
CFLAGS = -g -Wall -Werror -std=c11

SOURCES = $(wildcard src/*.c)
HEADERS = $(wildcard src/*.h)
OBJECTS = $(patsubst %.c, %.o, $(SOURCES))

EXECUTABLE = scoreboard

$(EXECUTABLE): $(OBJECTS) $(HEADERS)
	$(CC) $(CFLAGS) src/*.o -o $(EXECUTABLE)

clean:
	rm -f $(OBJECTS) ./$(EXECUTABLE)

.PHONY: clean
