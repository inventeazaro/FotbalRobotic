CFLAGS=-Wall -ggdb -I../../libs/mosquitto-1.0.5/lib
LDFLAGS=../../libs/mosquitto-1.0.5/lib/libmosquitto.so.1

.PHONY: all clean

all : move

move : move.o
	${CC} $^ -o $@ ${LDFLAGS}

move.o : move.c
	${CC} -c $^ -o $@ ${CFLAGS} -I../../lib

clean : 
	-rm -f *.o move
