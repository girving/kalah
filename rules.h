// Header file for mancala rules
// Geoffrey Irving
// 3/24/98

#ifndef __RULES_H
#define __RULES_H

#include <stdio.h>
#include <string.h>
#include "params.h"

class position {
  public:
    short w;
    char a[TPITS];

    position() {}
    position(int n);
    int move(int side, int bin);

    int rate(int s) { return s ? a[LPIT]-a[PITS] : a[PITS]-a[LPIT]; }
    int bin(int s, int b) { return s ? a[b+PITS+1] : a[b]; }
  };

extern void printq(FILE *f, int s, position p);
extern void print(FILE *f, int s, position p);

// Note:
//   Values are stored in chars for copying speed.
//   For hashing purposes, bins are restricted to 31 stones,
//   i.e., 5 bits.  This allows an entire board to be compressed
//   into a single 64 bit value, with 4 bits to spare.  Mancalas
//   are allowed the full 0-255 8-bit range, since they are not 
//   included in the hashing.  


#endif


