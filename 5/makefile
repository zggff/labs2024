CC = clang
CFLAGS = -std=gnu99 -pedantic -Wall -Wextra -Werror
TARGET = main.out

.PHONY: clean run build

build: ${TARGET}

${TARGET}: main.c
	${CC} ${CFLAGS} -o $@ $^

clean:
	@rm -f ./*.o
	@rm -f ${TARGET}
