// Endgame database class code file
// Geoffrey Irving
// 06sept99 

#include "endgame.h"

#define INIT_TABLE_MACRO(a,b,c)					\
  int i,j;							\
  for (i=0;i<PITS-1;i++) {					\
    c[i][0] = c[i+PITS+1][0] = 0;				\
    c[i][1] = c[i+PITS+1][1] = 1;				\
    for (j=1;j<40;j++)						\
      c[i][j+1] = c[i+PITS+1][j+1] = c[i][j] * (i+j+1) / j;	\
    }								\
								\
  for (i=0;i<40;i++)						\
    b[i] = c[PITS-2][i+1];					\
								\
  a[0][0] = a[1][1] = 0;					\
  for (i=2;i<=40;i++) {						\
    a[i][1] = a[i-1][i-1];					\
    for (j=1;j<i;j++)						\
      a[i][j+1] = a[i][j] + b[j] * b[i-j];			\
    if (a[i][i] & 7)						\
      a[i][i] = (a[i][i] & ~7) + 8;				\
    }

void eg_init_tables(endgame *e) {
  INIT_TABLE_MACRO(e->ai,e->bi,e->ci)
  }

void eg_init_long_tables(long long a[][50], long long b[], long long c[][50]) {
  INIT_TABLE_MACRO(a,b,c)
  }

void eg_create(char *file, int bits) {
  FILE* f = fopen(file,"w");
  endgame_header h;
  memcpy(h.sig,"eg",2);
  h.n = h.size = 0;
  h.bits = bits;
  fwrite(&h,sizeof(endgame_header),1,f);
  fclose(f);
  }
    
void read_endgame(endgame *e, FILE *f, int n) {
  endgame_header h;
  eg_init_tables(e);
  fread(&h,sizeof(endgame_header),1,f);
  if (memcmp(h.sig,"eg",2) || h.bits < 4 || h.bits > 5  
          || h.size != e->ai[h.n][h.n]*h.bits>>3) {
    fprintf(stderr,"Corrupt endgame database\n");
    exit(-1);
    }

  e->n = n > h.n ? h.n : n;
  e->bits = h.bits;
  e->size = e->ai[e->n][e->n] * e->bits >> 3;
  e->d = malloc(e->size);
  fread(e->d,sizeof(char),e->size,f);
  }
 
void write_endgame(endgame *e, FILE *f) {
  endgame_header h;
  memcpy(h.sig,"eg",2);
  h.n = e->n;
  h.bits = e->bits;
  h.size = e->size;
  fwrite(&h,sizeof(endgame_header),1,f);
  fwrite(e->d,sizeof(char),e->size,f);
  }
 
void free_endgame(endgame *e) {
  free(e->d);  
  }

size_t eg_index(endgame *e, int t, position *p) {
  register int i,k;
  register size_t r;
  if (p->s) {
    r = 0; 
    k = p->a[PITS+1];
    for (i=PITS+1;i<LPIT-1;) {
      r += e->ci[i][k];
      k += p->a[++i];
      }
    r = r * e->bi[t-k] + e->ai[t][k];
    k = 0;
    for (i=0;i<PITS-1;i++) {
      k += p->a[i];
      r += e->ci[i][k];
      }
    return r; 
    }
  else {
    r = 0;
    k = p->a[0];
    for (i=0;i<PITS-1;) {
      r += e->ci[i][k];
      k += p->a[++i];
      }
    r = r * e->bi[t-k] + e->ai[t][k];
    k = 0;
    for (i=PITS+1;i<LPIT-1;i++) {
      k += p->a[i];
      r += e->ci[i][k];
      }
    return r;
    }
  }

 
