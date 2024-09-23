CC = clang
CFLAGS = -std=gnu99 -pedantic -Wall -Wextra -Werror
TARGET = main.out

.PHONY: clean run build

build: ${TARGET}

${TARGET}: main.o ops.o
	${CC} -o $@ $^

main.o: main.c ops.h
	${CC} ${CFLAGS} -c -o $@ $<

ops.o: ops.c ops.h
	${CC} ${CFLAGS} -c -o $@ $<

clean:
	@rm -f ./*.o
	@rm -f ${TARGET}
