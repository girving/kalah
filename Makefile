# Makefile for mancala programs
# Geoffrey Irving
# 25jun00

CFLAGS = -O3 -funroll-loops -Winline
CILK = cilk# -cilk-profile -cilk-critical-path
ELIDE = -DNOCILK -x c
HDR = elision.h params.h rules.h

all: kalah generator twiddle
serial: kalah-s

kalah: kalah.o crunch.o hash.o endgame.o rules.o
	$(CILK) $(CFLAGS) -o kalah kalah.o crunch.o hash.o endgame.o rules.o

kalah-s: kalah-s.o crunch-s.o hash-s.o endgame.o rules.o
	gcc $(CFLAGS) -o kalah-s kalah-s.o crunch-s.o hash-s.o endgame.o rules.o

generator: generator.o rules.o endgame.o
	gcc $(CFLAGS) -o generator generator.o rules.o endgame.o

twiddle: twiddle.o rules.o endgame.o
	gcc $(CFLAGS) -o twiddle twiddle.o rules.o endgame.o


kalah.o: $(HDR) crunch.cilkh kalah.cilk
	$(CILK) $(CFLAGS) -c kalah.cilk 

kalah-s.o: $(HDR) crunch.cilkh kalah.cilk
	gcc $(CFLAGS) -c -o kalah-s.o $(ELIDE) kalah.cilk

generator.o: $(HDR) endgame.h generator.c
	gcc $(CFLAGS) -c generator.c

twiddle.o: $(HDR) twiddle.c
	gcc $(CFLAGS) -c twiddle.c 

rules.o: $(HDR) rules.c
	gcc $(CFLAGS) -c rules.c

crunch.o: $(HDR) mix.h hash.cilkh endgame.h crunch.cilkh crunch.cilk
	$(CILK) $(CFLAGS) -c crunch.cilk

crunch-s.o: $(HDR) mix.h hash.cilkh endgame.h crunch.cilkh crunch.cilk
	gcc $(CFLAGS) -c -o crunch-s.o $(ELIDE) crunch.cilk

hash.o: $(HDR) mix.h hash.cilkh hash.cilk
	$(CILK) $(CFLAGS) -c hash.cilk

hash-s.o: $(HDR) mix.h hash.cilkh hash.cilk
	gcc $(CFLAGS) -c -o hash-s.o $(ELIDE) hash.cilk

endgame.o: $(HDR) endgame.h endgame.c
	gcc $(CFLAGS) -c endgame.c

clean:
	rm *.o kalah kalah-s generator twiddle

