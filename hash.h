// Mancala knowledge hash class header file
// Geoffrey Irving
// 29july99

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

struct datum {
  ub4 b1, b2;
  signed char r; 
  unsigned char d,m,f;
  };

// Datum flags
#define D_TYPE 3
#define DV_INVALID 0
#define DV_LO 1
#define DV_HI 2
#define DV_EXACT 3
#define D_STALE 4 

class datumaccess { 
    friend class hash;
 
    datum* c;
    ub4 b1, b2;
    int ko, cf, br, cr;
    unsigned char cd,cm;
    int collision; 
  public:
    int gett() { return cf; }
    int getd() { return cd; } 
    int getm() { return cm; }
    int getr() { return cr; } 

    int seta(int d, int m, int r);
    void sett(int t) { c->f = t; }

    void verify();
  };

class hash {
    int bits;        // logarithmic size of hash table 
    long size;       // size of hash table (2^bits)
    long entries;    // number of entries 
    long mask;       // hash mask (size-1)

    datum* d;        // hash table 
  
  public:
    hash(int bitsize);             
    hash(FILE* f);       
    void save(FILE* f);
    ~hash();           

    void setstale();  // set staleness flag for all entries    
    void lookup(datumaccess *da, int s, position *p); 
  }; 

inline void datumaccess::verify() {
  if (ko) return;
  int t = c->f;
  if (t && c->b1 == b1 && c->b2 == b2) {
    cf = t & D_TYPE;
    c->f = cf;
    cr = br + c->r;
    cd = c->d;
    cm = c->m;
    }
  else
    cf = 0;
  }

#endif


