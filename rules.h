// Header file for kalah rules
// Geoffrey Irving
// 4sep0

#ifndef __RULES_H
#define __RULES_H

#include <stdio.h>
#include <string.h>
#include "params.h"

typedef struct {
  char s;
  signed char w;  
  char a[TPITS];
  } position;

extern void fill_pos(position *p, int n);
extern int move(position *p, int bin);

static inline int a_rate(position *p, int s) {
  return s ? p->a[LPIT]-p->a[PITS] : p->a[PITS]-p->a[LPIT]; 
  }
static inline int a_bin(position *p, int s, int b) { 
  return s ? p->a[b+PITS+1] : p->a[b]; 
  }

static inline int rate(position *p) { return a_rate(p,p->s); }
static inline int o_rate(position *p) { return a_rate(p,!p->s); }
static inline int bin(position *p, int b) { return a_bin(p,p->s,b); }
static inline int o_bin(position *p, int b) { return a_bin(p,!p->s,b); }

extern void write_pq(FILE *f, position p);
extern void write_p(FILE *f, position p, int s);

// Note:
//   Values are stored in chars for copying speed.
//   For hashing purposes, bins are restricted to 31 stones,
//   i.e., 5 bits.  This allows an entire board to be compressed
//   into a single 64 bit value, with 4 bits to spare.  Kalahahs
//   are allowed the full 0-255 8-bit range, since they are not 
//   included in the hashing.  

#endif


