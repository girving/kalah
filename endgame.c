// Endgame database class code file
// Geoffrey Irving
// 06sept99 

#include "endgame.h"

void eg_init_tables(endgame *e) {
  int i,j,s,t;
  for (i=0;i<PITS-1;i++) {
    e->ci[i][0] = e->ci[i+PITS+1][0] = 0;
    e->ci[i][1] = e->ci[i+PITS+1][1] = 1;
    for (j=1;j<40;j++)
      e->ci[i][j+1] = e->ci[i+PITS+1][j+1] = e->ci[i][j] * (i+j+1) / j; 
    }

  for (i=0;i<40;i++)
    e->bi[i] = e->ci[PITS-2][i+1];

  e->ai[0][0] = e->ai[1][1] = 0;
  e->si[0] = e->si[1] = 0;
  for (i=2;i<=40;i++) {
    e->ai[i][1] = e->ai[i-1][i-1];
    for (j=1;j<i;j++)
      e->ai[i][j+1] = e->ai[i][j] + e->bi[j] * e->bi[i-j];
    if (e->ai[i][i] & 7)
      e->ai[i][i] = (e->ai[i][i] & ~7) + 8;
    e->si[i] = (e->ai[i][i] - e->ai[i][1]) >> 1;
    }

  s = 0;
  for (i=2;i<=e->n;i++) {
    e->di[i][1] = s;
    if (e->nm[i]) {
      for (j=2;j<=i;j++)
        e->di[i][j] = e->di[i][1] + e->ai[i][j] - e->ai[i][1]; 
      s += e->si[i] << 1;
      }
    else
      e->di[i][i] = s;
    }
  }
    
void read_endgame(endgame *e, FILE *f, int n) {
  int i,nf;
  long t;
  e->n = n;
  for (i=0;i<=e->n;i++)
    e->nm[i] = 1;
  eg_init_tables(e);
  fread(&nf,sizeof(int),1,f); 
  if (e->n > nf)
    e->n = nf;
  e->size = e->di[e->n][e->n] >> 1;
  fread(&t,sizeof(long),1,f);
  if (t != e->ai[nf][nf] >> 1) {
    fprintf(stderr,"Corrupt endgame database\n");
    exit(-1);
    }
  e->d = (unsigned char*) malloc(e->size);
  t = sizeof(int) + sizeof(long);
  for (i=2;i<=e->n;i++)
    if (e->nm[i]) {
      fseek(f,t+(e->ai[i][1]>>1),SEEK_SET);  
      fread(e->d+(e->di[i][1]>>1),sizeof(char),e->si[i],f);
      }
  }
 
extern void write_endgame(endgame *e, FILE *f) {
  fwrite(&e->n,sizeof(int),1,f); 
  fwrite(&e->size,sizeof(long),1,f);
  if (fwrite(e->d,sizeof(char),e->size,f) != e->size)
    exit(-1);
  }
 
extern void free_endgame(endgame *e) {
  free(e->d);  
  }

extern index_t eg_index(endgame *e, int t, position *p) {
  register int i,k;
  register index_t r;
  if (p->s) {
    r = 0; 
    k = p->a[PITS+1];
    for (i=PITS+1;i<LPIT-1;) {
      r += e->ci[i][k];
      k += p->a[++i];
      }
    r = r * e->bi[t-k] + e->di[t][k];
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
    r = r * e->bi[t-k] + e->di[t][k];
    k = 0;
    for (i=PITS+1;i<LPIT-1;i++) {
      k += p->a[i];
      r += e->ci[i][k];
      }
    return r;
    }
  }

 
