/* Endgame position classifier
 * Geoffrey Irving
 * $Id$ */

#include <stdlib.h>
#include <math.h>
#include "rules.h"
#include "endgame.h"

/******** decision trees */

#define STUMP 0
#define LEAF 1
#define INCOMPLETE 2

typedef struct _dtree {
  char kind; /* type of dtree */
  char d;    /* dimension */
  char t;    /* threshold */
  char v;    /* value */
  union {
    struct _dtree *c[2];  /* children */
    int *ct;              /* pointer to count data */
    } a;
  } dtree;

dtree *new_leaf() {
  dtree *p = malloc(sizeof(dtree));
  p->kind = INCOMPLETE;
  p->a.ct = calloc(12 * 32 * 32, sizeof(int));
  return p;
  } 

void expand_leaf(dtree *p, int d, int t) {
  p->kind = STUMP;
  free(p->a.ct);
  p->d = d;
  p->t = t;
  p->a.c[0] = new_leaf();
  p->a.c[1] = new_leaf();
  }

void finish_leaf(dtree *p, int v) {
  p->kind = LEAF; 
  free(p->a.ct);
  p->v = v;
  }

int *decide_count(dtree *t, position *p) {
  if (t->kind == INCOMPLETE)
    return t->a.ct;
  else if (t->kind == LEAF)
    return 0;
  else if (p->a[(int)t->d] > t->t)
    return decide_count(t->a.c[0], p);
  else
    return decide_count(t->a.c[1], p);
  }

int dtree_size(dtree *p) {
  if (p->kind == STUMP)
    return 1 + dtree_size(p->a.c[0]) + dtree_size(p->a.c[1]);
  else
    return 1;
  }

/******** decision tree generation */

void count(int n, dtree *t) {
  void f(position *p) {
    int i,v; 
    int (*c)[32][32];
    v = eg_getd(eg_index(n, p));
    c = (int(*)[32][32])decide_count(t, p);
    if (c)
      for (i=0;i<12;i++)
        c[i][(int)p->a[i]][(int)v]++;
    }
  position_iterate(f, n);
  }

void compute_stump(dtree *dt) {
  /* recurse if necessary */
  if (dt->kind == STUMP) {
    compute_stump(dt->a.c[0]);
    compute_stump(dt->a.c[1]);
    return;
    }
  else if (dt->kind == LEAF)
    return;

  /* grab count information */
  int (*c)[32][32] = (int(*)[32][32])dt->a.ct;

  /* compute the total number of positions */
  int i,j;
  int total = 0;
  for (i=0;i<32;i++)
    for (j=0;j<32;j++)
      total += c[0][i][j];

  /* compute the probabilities of each minimax value */
  double p[32] = {}; 
  for (i=0;i<32;i++) {
    int tp = 0;
    for (j=0;j<32;j++)
      tp += c[0][j][i];
    if (tp == total) {
      finish_leaf(dt, i);
      return;
      }
    p[i] = (double)tp / total; 
    }
      
  /* compute total information */
  double Ix = 0;
  for (i=0;i<32;i++)
    Ix += p[i] > 0 ? -p[i] * log(p[i]) : 0; 
  Ix *= M_LOG2E;
  if (Ix == 0) {
    finish_leaf(dt, 7);
    return;
    }

  /* current optimal decision stump */
  int md, mt;
  double mI;
  mI = 0;
  md = mt = -1;

  /* loop over all possible decision stumps */
  int d, t; 
  double I;
  for (d=0;d<12;d++)
    for (t=0;t<32;t++) {
      /* compute probabilities of stump returning 0 / 1 */
      double ps[2];
      ps[0] = ps[1] = 0;
      for (i=0;i<32;i++)
        for (j=0;j<32;j++)
          ps[i > t] += c[d][i][j];
      ps[0] /= total;
      ps[1] /= total;
      if (ps[0] == 0 || ps[1] == 0)
        continue;

      /* compute joint probabilities */
      double pj[2][32] = {};
      for (i=0;i<32;i++)
        for (j=0;j<32;j++)
          pj[i > t][j] += c[d][i][j];
      for (j=0;j<32;j++) {
        pj[0][j] /= total;
        pj[1][j] /= total;
        }

      /* compute mutual information */
      I = 0;
      for (i=0;i<2;i++)
        for (j=0;j<32;j++)
          if (pj[i][j] > 0)
            I += pj[i][j] * log(pj[i][j] / (p[j] * ps[i]));
      I *= M_LOG2E;
      
      /* new optimum? */
      if (I > mI) {
        mI = I;
        md = d;
        mt = t;
        }
      }

  expand_leaf(dt, md, mt);

  /* print results */
/*
  printf("Decision stump: d %d, t %d\n", md, mt);
  printf("Information: total %g, mutual %g, ratio %g\n", Ix, mI, Ix / mI);
*/
  }

void grow_dtree(int n, dtree *dt) {
  count(n, dt);
  compute_stump(dt);
  }
        
/******** toplevel */

void init_endgame(int size, char *file) {
  if (!access(file,F_OK)) {          // endgame database exists
    FILE *f = fopen(file,"r");
    read_endgame(f,size);
    fclose(f);
    }
  else {
    fprintf(stderr,"Cannot open endgame database\n");
    exit(-1);
    }
  }

#define N 5

int main() {
  init_endgame(N, "endgame5.dat");
  dtree *dt = new_leaf();
  for (;;) {
    grow_dtree(N, dt);
    printf("size %d\n", dtree_size(dt));
    printf("\n");
    }
  return 0;
  }
