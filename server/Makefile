.PHONY: all default clean

SRC := aesdsocket.c data_management.c
CC ?=${CROSS_COMPILE}gcc
CFLAGS ?=-O0 -g -Wall -DUSE_AESD_CHAR_DEVICE
TARGET ?= aesdsocket
LDFLAGS ?= -lpthread -lrt
OBJS := $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	${CC} ${CFLAGS} $(OBJS) -o $(TARGET) $(LDFLAGS)

clean:
	rm -rf *.o aesdsocket
