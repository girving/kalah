# Makefile for mancala programs
# Geoffrey Irving
# 25jun00

CFLAGS = -O3 -funroll-loops -Winline
HDR = params.h rules.h

all: kalah generator twiddle

kalah: kalah.o crunch.o rules.o hash.o endgame.o 
	gcc $(CFLAGS) -o kalah kalah.o crunch.o rules.o hash.o endgame.o 

generator: generator.o rules.o endgame.o
	gcc $(CFLAGS) -o generator generator.o rules.o endgame.o

twiddle: twiddle.o rules.o
	gcc $(CFLAGS) -o twiddle twiddle.o rules.o

kalah.o: $(HDR) crunch.h kalah.c
	gcc $(CFLAGS) -c kalah.c 

generator.o: $(HDR) endgame.h generator.c
	gcc $(CFLAGS) -c generator.c

twiddle.o: $(HDR) twiddle.c
	gcc $(CFLAGS) -c twiddle.c 

rules.o: $(HDR) rules.c
	gcc $(CFLAGS) -c rules.c

crunch.o: $(HDR) hash.h endgame.h crunch.h crunch.c
	gcc $(CFLAGS) -c crunch.c

hash.o: $(HDR) hash.h hash.c
	gcc $(CFLAGS) -c hash.c

endgame.o: $(HDR) endgame.h endgame.c
	gcc $(CFLAGS) -c endgame.c

clean:
	rm *.o kalah generator twiddle
