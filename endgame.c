// Endgame database class code file
// Geoffrey Irving
// 06sept99 

#include "endgame.h"

char eg_sig[] = "eg";

struct endgame eg;

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

void eg_init_tables() {
  INIT_TABLE_MACRO(eg.ai,eg.bi,eg.ci,40)
  }

void eg_init_long_tables(long long a[][100], long long b[], long long c[][100]) {
  INIT_TABLE_MACRO(a,b,c,90)
  }

void eg_create(char *file, int bits) {
  FILE* f = fopen(file,"w");
  struct endgame_header h;
  memcpy(h.sig,eg_sig,2);
  strcpy(h.n,"0");
  strcpy(h.size,"0");
  sprintf(h.bits,"%d",bits);
  fwrite(&h,sizeof(struct endgame_header),1,f);
  fclose(f);
  }
    
void read_endgame(FILE *f, int n) {
  struct endgame_header h;
  eg_init_tables();
  fread(&h,sizeof(struct endgame_header),1,f);
  if (memcmp(h.sig,eg_sig,2)) {
    fprintf(stderr,"Corrupt endgame database\n");
    exit(-1);
    }
  eg.n = atoi(h.n);
  eg.bits = atoi(h.bits);
  eg.size = strtoul(h.size,NULL,0);
  if (eg.size != eg.ai[eg.n][eg.n]*eg.bits>>3) {
    fprintf(stderr,"Corrupt endgame database\n");
    exit(-1);
    }

  if (eg.n > n) eg.n = n;
  eg.size = eg.ai[eg.n][eg.n] * eg.bits >> 3;
  eg.d = malloc(eg.size);
  fread(eg.d,sizeof(char),eg.size,f);
  }
 
void write_endgame(FILE *f) {
  struct endgame_header h;
  memcpy(h.sig,eg_sig,2);
  sprintf(h.n,"%-4d",eg.n);
  sprintf(h.bits,"%-4d",eg.bits);
  sprintf(h.size,"%-19lld",(long long)eg.size);
  fwrite(&h,sizeof(struct endgame_header),1,f);
  fwrite(eg.d,sizeof(char),eg.size,f);
  }
 
void free_endgame() {
  free(eg.d);  
  }

size_t eg_index(int t, position *p) {
  int i,k;
  size_t r;
  if (p->s) {
    r = 0; 
    k = p->a[PITS+1];
    for (i=PITS+1;i<LPIT-1;) {
      r += eg.ci[i][k];
      k += p->a[++i];
      }
    r = r * eg.bi[t-k] + eg.ai[t][k];
    k = 0;
    for (i=0;i<PITS-1;i++) {
      k += p->a[i];
      r += eg.ci[i][k];
      }
    return r; 
    }
  else {
    r = 0;
    k = p->a[0];
    for (i=0;i<PITS-1;) {
      r += eg.ci[i][k];
      k += p->a[++i];
      }
    r = r * eg.bi[t-k] + eg.ai[t][k];
    k = 0;
    for (i=PITS+1;i<LPIT-1;i++) {
      k += p->a[i];
      r += eg.ci[i][k];
      }
    return r;
    }
  }

/******** for generator use */ 

void position_iterate(void (*f)(position *p), int n) {
  position p;
  int i,j,k;
  p.s = 0;
  fill_pos(&p,0);

  i = n;
  printf("%2d) ",i);
  fflush(stdout);
  for (j=1;j<i;j++) { 
    p.a[PITS-1] = j;  
    for (;;) { 
      p.a[LPIT-1] = i-j;
      for (;;) { 
        f(&p);
        k = LPIT-1;
        while (!p.a[k]) k--;
        if (k == PITS+1) { 
          p.a[k] = 0;
          break;
          }
        p.a[k]--;
        p.a[k-1]++;
        while (k < LPIT-1) { 
          p.a[LPIT-1] += p.a[k];
          p.a[k++] = 0;
          }
        }
      k = PITS-1;
      while (!p.a[k]) k--; 
      if (!k) { 
        p.a[k] = 0;
        break;
        }
      p.a[k]--;
      p.a[k-1]++; 
      while (k < PITS-1) { 
        p.a[PITS-1] += p.a[k];
        p.a[k++] = 0;
        }
      }
    putchar('.');
    fflush(stdout);
    }
  putchar('\n');
  }

