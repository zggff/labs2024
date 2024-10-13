CC = clang
CFLAGS = -std=gnu17 -Wall -Wextra -g

.PHONY: clean run build

build: main.out

main.out: main.c lib.c
	${CC} ${CFLAGS} -o $@ $^

clean:
	@rm -f ./*.o
	@rm -f ./*.out
