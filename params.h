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

/* Iterative deepening: */ 
#define STEPSIZE 3
#define JUMPDEPTH 30

/* Optional stuff */
#define STAT(a) a

#endif
