.PHONY: all default clean

CC:=${CROSS_COMPILE}gcc
CFLAGS:=-Wall

all:
	${CC} -o aesdsocket *.c ${CFLAGS}

clean:
	rm -rf *.o aesdsocket
