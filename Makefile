CC=gcc

CFLAGS=-DDEBUG

C_SOURCES=$(wildcard src/*.c)

opulus: $(C_SOURCES)
	$(CC) $^ -o $@ $(CFLAGS)

clean:
	rm -f ./opulus

.PHONY: clean
