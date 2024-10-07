CC = clang
CFLAGS = -std=gnu17 -Wall -Wextra -g
TARGET = main.out

.PHONY: clean run build

build: ${TARGET}

${TARGET}: main.c
	${CC} ${CFLAGS} -o $@ $^

clean:
	@rm -f ./*.o
	@rm -f ${TARGET}
