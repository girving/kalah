// Various kalah program parameters
// Geoffrey Irving
// 8aug0

#ifndef __PARAMS_H
#define __PARAMS_H

#include "elision.h"

/* Basic rules: */
#define PITS 6            // pits on one side (PITS < 7)
#define TPITS (2*PITS+2)  // total pits
#define LPIT (TPITS-1)    // index of last pit
#define MAXGAMELEN 193    // bad things happen if wrong

/* Transposition tables: */
#define LOWDEPTH 3
#define HASH_32 32 
#define HASH_64 64
#define ZOBRIST 2
#define HASH 64

#ifdef CILK
#define LOCKBITS 10
#define LOCKSIZE (1<<LOCKBITS)
#define LOCKMASK (LOCKSIZE-1)
#define LOCKING
#endif

/* Iterative deepening: */ 
#define STEPSIZE 3
#define JUMPDEPTH 30

/* Optional stuff */
#define PROGRESS 1
#define STAT(a) a

#endif
