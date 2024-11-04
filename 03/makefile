CC = clang
CFLAGS = -std=gnu17 -pedantic -Wall -Wextra -g
TARGET = main.out

.PHONY: clean run build test

build: ${TARGET}

${TARGET}: main.c lib.c lib.h trie.c trie.h
	${CC} ${CFLAGS} -o $@ main.c lib.c trie.c

test: build
	@pypy3 ./test.py

clean:
	@rm -f ./*.o
	@rm -f ${TARGET}
