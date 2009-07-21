# Makefile for kalah programs
# Geoffrey Irving
# 28oct00 

CC = gcc
CILK = cilk
CFLAGS = -O3 -funroll-loops -Winline -Wall -fnested-functions
CILKFLAGS = $(CFLAGS) #-cilk-profile -cilk-critical-path

ELIDE = -DNOCILK -x c

obj = endgame.o rules.o
objects = kalah.o kalah-s.o crunch.o crunch-s.o hash.o hash-s.o generator.o twiddle.o classify.o $(obj) 
progs = generator twiddle classify

%-s.o: %.cilk
	$(CC) $(CFLAGS) -c -o $@ $(ELIDE) $<

%.o: %.cilk
	$(CILK) $(CILKFLAGS) $(CFLAGS) -c $<

%: %.o $(obj)
	$(CC) $(CFLAGS) -o $@ $^

serial: kalah-s $(progs)
cilk: kalah $(progs)
all: kalah kalah-s $(progs)

kalah: kalah.o crunch.o hash.o $(obj)
	$(CILK) $(CILKFLAGS) -o $@ $^

kalah-s: crunch-s.o hash-s.o
generator twiddle classify:

classify: classify.o $(obj)
	$(CC) $(CFLAGS) -o $@ $^ -lm

$(objects) : elision.h params.h rules.h

kalah.o kalah-s.o : crunch.cilkh
crunch.o crunch-s.o hash.o hash-s.o : mix.h hash.cilkh
crunch.o crunch-s.o generator.o endgame.o : endgame.h

.PHONY: clean
clean:
	rm -f *.o kalah kalah-s $(progs)

