# Makefile for kalah programs
# Geoffrey Irving
# 28oct00 

CC = gcc
CILK = cilk
CFLAGS = -funroll-loops -Winline -Wall
CILKFLAGS = $(CFLAGS) #-cilk-profile -cilk-critical-path

ELIDE = -DNOCILK -x c

obj = endgame.o rules.o
objects = kalah.o kalah-s.o crunch.o crunch-s.o hash.o hash-s.o generator.o twiddle.o $(obj) 

%-s.o: %.cilk
	$(CC) $(CFLAGS) -c -o $@ $(ELIDE) $<

%.o: %.cilk
	$(CILK) $(CILKFLAGS) $(CFLAGS) -c $<

%: %.o $(obj)
	$(CC) $(CFLAGS) -o $@ $^

all: kalah kalah-s generator twiddle
cilk: kalah generator twiddle
serial: kalah-s generator twiddle

kalah: kalah.o crunch.o hash.o $(obj)
	$(CILK) $(CILKFLAGS) -o $@ $^

kalah-s: crunch-s.o hash-s.o
generator twiddle:

$(objects) : elision.h params.h rules.h

kalah.o kalah-s.o : crunch.cilkh
crunch.o crunch-s.o hash.o hash-s.o : mix.h hash.cilkh
crunch.o crunch-s.o generator.o endgame.o : endgame.h

.PHONY: clean
clean:
	rm -f *.o kalah kalah-s generator twiddle

