// Endgame database class header file
// Geoffrey Irving
// 05sept99

#ifndef __ENDGAME_H
#define __ENDGAME_H

#include <stdio.h>
#include "params.h"
#include "rules.h"

#define EG_LO 1
#define EG_EXACT 3

class endgame {
    int n;
    unsigned long size;
    int nm[50];   
    unsigned char* d;

    unsigned long ai[50][50];
    unsigned long bi[50];
    unsigned long ci[LPIT][50];
    unsigned long di[50][50];
    unsigned long si[50];

    unsigned long index(int s, int t, position *p); 
    int getd(unsigned long i);
    void setd(unsigned long i, int v);
    
    void initialize();

    friend class generator;
    void save(FILE *f);
  
  public:
    endgame(FILE *f, int nv, int *mv = 0);
    ~endgame();

    int stones() { return n; }
    unsigned long memory() { return size; }
    int known(int t) { return t > n ? 0 : nm[t]; }
    int lookup(int s, int t, position *p, int *q);
  };

inline int endgame::getd(unsigned long i) {
  return i&1 ? d[i>>1]>>4 : d[i>>1] & 15;
  }

inline void endgame::setd(unsigned long i, int v) {
  if (--v > 15) v = 15;
  d[i>>1] = i&1 ? d[i>>1] & 15 | (v << 4) : d[i>>1] & 240 | v;
  }

inline int endgame::lookup(int s, int t, position *p, int *q) { 
  int r = getd(index(s,t,p));
  *q = r < 15 ? EG_EXACT : EG_LO;
  return (++r << 1) - t + p->rate(s);
  }

#endif
    

