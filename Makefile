# Makefile for mancala programs
# Geoffrey Irving
# 25jun00

CFLAGS = -O3 -funroll-loops
HDR = params.h rules.h

all: kalah generator

kalah: kalah.o crunch.o rules.o hash.o endgame.o 
	gcc $(CFLAGS) -o kalah kalah.o crunch.o rules.o hash.o endgame.o 

generator: generator.o rules.o endgame.o
	gcc $(CFLAGS) -o generator generator.o rules.o endgame.o

kalah.o: $(HDR) crunch.h kalah.c
	gcc -c $(CFLAGS) kalah.c 

generator.o: $(HDR) endgame.h generator.c
	gcc -c $(CFLAGS) generator.c

rules.o: $(HDR) rules.c
	gcc -c $(CFLAGS) rules.c

crunch.o: $(HDR) hash.h endgame.h crunch.h crunch.c
	gcc -c $(CFLAGS) crunch.c

hash.o: $(HDR) hash.h hash.c
	gcc -c $(CFLAGS) hash.c

endgame.o: $(HDR) endgame.h endgame.c
	gcc -c $(CFLAGS) endgame.c

clean:
	rm *.o kalah generator
