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

kalah.o: $(HDR) crunch.h kalah.cpp
	gcc -c $(CFLAGS) kalah.cpp 

generator.o: $(HDR) endgame.h generator.cpp
	gcc -c $(CFLAGS) generator.cpp

rules.o: $(HDR) rules.cpp
	gcc -c $(CFLAGS) rules.cpp

crunch.o: $(HDR) hash.h endgame.h crunch.h crunch.cpp
	gcc -c $(CFLAGS) crunch.cpp

hash.o: $(HDR) hash.h hash.cpp
	gcc -c $(CFLAGS) hash.cpp

endgame.o: $(HDR) endgame.h endgame.cpp
	gcc -c $(CFLAGS) endgame.cpp

clean:
	rm *.o kalah generator
