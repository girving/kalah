// Kalah minimax search code
// Geoffrey Irving
// 1sep0 

#ifndef __CRUNCH_H
#define __CRUNCH_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "params.h"
#include "rules.h"
#include "hash.h"
#include "endgame.h"

#define MAXMVC 20       // maximum number of moves to calculate

/* Search flags */
#define SF_GUESS   1      // use guess value 
#define SF_JUMP    2      // skip iterative deepening
#define SF_SINGLE  4      // execute only one test call at full depth
#define SF_VERBOSE 8      // verbose output
#define SF_FLOOD  16      // extremely verbose output
#define SF_STALE  32      // set stale flags at end of search
#define SF_FULL  128      // internal use only 

/* global variables */
extern int stones; 
extern hash* ha;     
extern endgame* eg; 

/* Job statistics */
struct stat_t {
  int maxdepth;
  double elapsed;
  long long nodes, peeks, cutoffs;
  long long ha_lookups, eg_lookups, f_cutoffs;
  };

extern stat_t stat;
extern void init_stat();
extern void print_stat(FILE *f, char *p);

/* Top level search routine */
extern int solve(int s, char *m, position *p, int d, int *rd, int g, int f);

/* Miscellaneous routines */
extern void init_hash(int size, char *file);
extern void save_hash(char *file);
extern void close_hash();
extern void init_endgame(int size, char *file);
extern void close_endgame();

#endif
