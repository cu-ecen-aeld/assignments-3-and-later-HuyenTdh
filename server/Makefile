.PHONY: all default clean

CC:=${CROSS_COMPILE}gcc
CFLAGS:=-O0 -g -Wall -lpthread -lrt

all:
	${CC} -o aesdsocket *.c ${CFLAGS}

clean:
	rm -rf *.o aesdsocket
