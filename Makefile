# Makefile for mancala programs
# Geoffrey Irving
# 25jun00

CC = gcc
CILK = cilk# -cilk-profile -cilk-critical-path
CFLAGS = -O3 -funroll-loops -Winline

ELIDE = -DNOCILK -x c

obj = endgame.o rules.o
kalah-obj = kalah.o crunch.o hash.o $(obj)
serial-obj = kalah-s.o crunch-s.o hash-s.o $(obj)
generator-obj = generator.o $(obj)
twiddle-obj = twiddle.o $(obj)

%-s.o: %.cilk
	$(CC) $(CFLAGS) -c -o $@ $(ELIDE) $<

%.o: %.cilk
	$(CILK) $(CFLAGS) -c $<

define build
$(CC) $(CFLAGS) -o $@ $^
endef

define build-cilk
$(CILK) $(CFLAGS) -o $@ $^
endef

all: kalah generator twiddle
serial: kalah-s

kalah: $(kalah-obj) 
	$(build-cilk)
kalah-s: $(serial-obj)
	$(build)
generator: $(generator-obj)
	$(build)
twiddle: $(twiddle-obj)
	$(build)

$(kalah-obj) $(serial-obj) $(generator-obj) $(twiddle-obj) : elision.h params.h rules.h
kalah.o kalah-s.o : crunch.cilkh
crunch.o crunch-s.o hahs.o hash-s.o : mix.h hash.cilkh
crunch.o crunch-s.o generator.o endgame.o : endgame.h

.PHONY: clean
clean:
	rm *.o kalah kalah-s generator twiddle

