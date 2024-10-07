CC = clang
CFLAGS = -std=gnu17 -Wall -Wextra -g
TARGET = main.out

.PHONY: clean run build

build: ${TARGET}

string.out: string.o lib.o
	${CC} ${CFLAGS} -o $@ $^

main.out: main.o lib.o
	${CC} ${CFLAGS} -o $@ $^

main.o: main.c lib.h
	${CC} ${FLAGS} -c -o $@ $<

string.o: string.c lib.h
	${CC} ${FLAGS} -c -o $@ $<

lib.o: lib.c lib.h
	${CC} ${FLAGS} -c -o $@ $<

clean:
	@rm -f ./*.o
	@rm -f ${TARGET}
