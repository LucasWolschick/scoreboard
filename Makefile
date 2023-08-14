CC = gcc
CFLAGS = -g

SOURCES = $(wildcard src/*.c)
HEADERS = $(wildcard src/*.h)
OBJECTS = $(patsubst %.c, %.o, $(SOURCES))

EXECUTABLE = scoreboard

$(EXECUTABLE): $(OBJECTS) $(HEADERS)
	$(CC) src/*.o -o $(EXECUTABLE)

clean:
	rm -f $(OBJECTS) ./$(EXECUTABLE)

.PHONY: clean
