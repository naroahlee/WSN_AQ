INCLUDE_DIR := ./inc
SRC_PATH := ./src
CFLAG = -I${INCLUDE_DIR} -Wall
CC = /usr/bin/gcc
LDLIBS = -lm -lxbee -lpthread -lrt

vpath %.c ${SRC_PATH}


.PHONY: all clean install
ALL = wsn_aqm data_parser

all: ${ALL}

clean:
	rm -rf ${ALL} *.o
	rm -rf ./Data/*

install:
	cp ${ALL} ./bin

%.o: %.c
	${CC} ${CFLAG} -c $^ -o $@

wsn_aqm: wsn_aqm.o data_producer.o data_consumer.o cyclic_util.o
	${CC} ${LDLIBS} ${CFLAG} $^ -o $@

data_parser: data_parser.o
	${CC} ${LDLIBS} ${CFLAG} $^ -o $@
