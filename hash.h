// Kalah knowledge hash class header file
// Geoffrey Irving
// 4sep0

#ifndef __HASH_H
#define __HASH_H

#include <stdlib.h>
#include <stdio.h> 
#include "params.h"
#include "rules.h"

typedef unsigned short ub2;
typedef unsigned long ub4;

#define hashsize(n) ((ub4)1<<(n))
#define hashmask(n) (hashsize(n)-1)

typedef struct __datum {
  ub4 b1, b2;
  signed char r; 
  unsigned char d,m,f;
  } datum;

// Datum flags
#define D_TYPE 3
#define DV_INVALID 0
#define DV_LO 1
#define DV_HI 2
#define DV_EXACT 3
#define D_STALE 4 

typedef struct __datumaccess {
  datum *c;
  ub4 b1, b2;
  int ko, br;
  int t, r;
  unsigned char d,m;
  int collision; 
  } datumaccess;

extern int da_seta(datumaccess *da, int d, int m, int r);
static inline void da_sett(datumaccess *d, int t) { d->c->f = t; }
static inline void da_verify(datumaccess *d);


typedef struct __hash {
  int bits;        // logarithmic size of hash table 
  long size;       // size of hash table (2^bits)
  long entries;    // number of entries 
  long mask;       // hash mask (size-1)

  datum *d;        // hash table 
  } hash;

extern void create_hash(hash *h, int bitsize);
extern void read_hash(hash *h, FILE *f);
extern void write_hash(hash *h, FILE *f);
extern void free_hash(hash *h);

extern void setstale(hash *h);
extern void ha_lookup(hash *h, datumaccess *da, position *p);


/* inline code */

static inline void da_verify(datumaccess *d) {
  int t;
  if (d->ko) return;
  t = d->c->f;
  if (t && d->c->b1 == d->b1 && d->c->b2 == d->b2) {
    d->t = t & D_TYPE;
    d->c->f = d->t;
    d->r = d->br + d->c->r;
    d->d = d->c->d;
    d->m = d->c->m;
    }
  else
    d->t = 0;
  }

#endif


