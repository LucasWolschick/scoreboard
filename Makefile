CC = gcc
CFLAGS = -g -fsanitize=address -fsanitize=undefined -Wall -Werror

SOURCES = $(wildcard src/*.c)
HEADERS = $(wildcard src/*.h)
OBJECTS = $(patsubst %.c, %.o, $(SOURCES))

EXECUTABLE = scoreboard

$(EXECUTABLE): $(OBJECTS) $(HEADERS)
	$(CC) $(CFLAGS) src/*.o -o $(EXECUTABLE)

clean:
	rm -f $(OBJECTS) ./$(EXECUTABLE)

.PHONY: clean
