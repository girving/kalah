// Endgame database class code file
// Geoffrey Irving
// 06sept99 

#include "endgame.h"

char eg_sig[] = "eg";

#define INIT_TABLE_MACRO(a,b,c,n)				\
  int i,j;							\
  for (i=0;i<PITS-1;i++) {					\
    c[i][0] = c[i+PITS+1][0] = 0;				\
    c[i][1] = c[i+PITS+1][1] = 1;				\
    for (j=1;j<n;j++)						\
      c[i][j+1] = c[i+PITS+1][j+1] = c[i][j] * (i+j+1) / j;	\
    }								\
								\
  for (i=0;i<n;i++)						\
    b[i] = c[PITS-2][i+1];					\
								\
  a[0][0] = a[1][1] = 0;					\
  for (i=2;i<=n;i++) {						\
    a[i][1] = a[i-1][i-1];					\
    for (j=1;j<i;j++)						\
      a[i][j+1] = a[i][j] + b[j] * b[i-j];			\
    if (a[i][i] & 7)						\
      a[i][i] = (a[i][i] & ~7) + 8;				\
    }

void eg_init_tables(endgame *e) {
  INIT_TABLE_MACRO(e->ai,e->bi,e->ci,40)
  }

void eg_init_long_tables(long long a[][100], long long b[], long long c[][100]) {
  INIT_TABLE_MACRO(a,b,c,90)
  }

void eg_create(char *file, int bits) {
  FILE* f = fopen(file,"w");
  endgame_header h;
  memcpy(h.sig,eg_sig,2);
  strcpy(h.n,"0");
  strcpy(h.size,"0");
  sprintf(h.bits,"%d",bits);
  fwrite(&h,sizeof(endgame_header),1,f);
  fclose(f);
  }
    
void read_endgame(endgame *e, FILE *f, int n) {
  endgame_header h;
  eg_init_tables(e);
  fread(&h,sizeof(endgame_header),1,f);
  if (memcmp(h.sig,eg_sig,2)) {
    fprintf(stderr,"Corrupt endgame database\n");
    exit(-1);
    }
  e->n = atoi(h.n);
  e->bits = atoi(h.bits);
  e->size = strtoul(h.size,NULL,0);
  if (e->size != e->ai[e->n][e->n]*e->bits>>3) {
    fprintf(stderr,"Corrupt endgame database\n");
    exit(-1);
    }

  if (e->n > n) e->n = n;
  e->size = e->ai[e->n][e->n] * e->bits >> 3;
  e->d = malloc(e->size);
  fread(e->d,sizeof(char),e->size,f);
  }
 
void write_endgame(endgame *e, FILE *f) {
  endgame_header h;
  memcpy(h.sig,eg_sig,2);
  sprintf(h.n,"%-4d",e->n);
  sprintf(h.bits,"%-4d",e->bits);
  sprintf(h.size,"%-19lld",(long long)e->size);
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

 
