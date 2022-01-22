SRC_FILES= main.c
CC_FLAGS = -Wall -Wextra -g -std=c11 -lm `sdl2-config --libs --cflags` -lpthread
CC = gcc
OUT = vm

all:
	${CC} ${SRC_FILES} ${CC_FLAGS} -o ${OUT}

clean:
	rm ${OUT}
