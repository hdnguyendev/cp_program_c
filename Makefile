cc = gcc
CFLAGS = -Wall -Wextra

EXECUTABLE = cp

all: ${EXECUTABLE}

${EXECUTABLE}: cp.c
	${CC} ${CFLAGS} -o $@ $<

.PHONY: clean run

clean: 
	rm -f ${EXECUTABLE}
run: ${EXECUTABLE}
	./${EXECUTABLE}