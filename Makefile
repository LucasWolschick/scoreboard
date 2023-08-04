CC = clang
CFLAGS = -g

parser: src/main.o src/parser.o src/vector.o src/parser.h src/vector.h
	$(CC) src/*.o -o parser

clean:
	rm -f src/*.o ./parser

.PHONY: clean
