// Endgame database class code file
// Geoffrey Irving
// 06sept99 

#include "endgame.h"

void endgame::initialize() {
  int i,j,s,t;
  for (i=0;i<PITS-1;i++) {
    ci[i][0] = ci[i+PITS+1][0] = 0;
    ci[i][1] = ci[i+PITS+1][1] = 1;
    for (j=1;j<40;j++)
      ci[i][j+1] = ci[i+PITS+1][j+1] = ci[i][j] * (i+j+1) / j; 
    }

  for (i=0;i<40;i++)
    bi[i] = ci[PITS-2][i+1];

  ai[0][0] = ai[1][1] = 0;
  si[0] = si[1] = 0;
  for (i=2;i<=40;i++) {
    ai[i][1] = ai[i-1][i-1];
    for (j=1;j<i;j++)
      ai[i][j+1] = ai[i][j] + bi[j] * bi[i-j];
    if (ai[i][i] & 1)
      ai[i][i]++;
    si[i] = (ai[i][i] - ai[i][1]) >> 1;
    }

  s = 0;
  for (i=2;i<=n;i++) {
    di[i][1] = s;
    if (nm[i]) {
      for (j=2;j<=i;j++)
        di[i][j] = di[i][1] + ai[i][j] - ai[i][1]; 
      s += si[i] << 1;
      }
    else
      di[i][i] = s;
    }

  }
    
endgame::endgame(FILE* f, int nv, int* mv) {
  int i,nf;
  long t;
  if (mv) {
    n = 0;
    for (i=0;i<nv;i++)
      if (nm[i] = mv[i])
        n = i;
    }
  else {
    n = nv;
    for (i=0;i<=nv;i++)
      nm[i] = 1;
    }
  initialize();
  fread(&nf,sizeof(int),1,f); 
  if (n > nf)
    n = nf;
  size = di[n][n] >> 1;
  fread(&t,sizeof(long),1,f);
  if (t != ai[nf][nf] >> 1) {
    fprintf(stderr,"Corrupt endgame database\n");
    exit(-1);
    }
  d = new unsigned char[size];
  t = sizeof(int) + sizeof(long);
  for (i=2;i<=n;i++)
    if (nm[i]) {
      fseek(f,t+(ai[i][1]>>1),SEEK_SET);  
      fread(d+(di[i][1]>>1),sizeof(char),si[i],f);
      }
  }
 
void endgame::save(FILE* f) {
  fwrite(&n,sizeof(int),1,f); 
  fwrite(&size,sizeof(long),1,f);
  if (fwrite(d,sizeof(char),size,f) != size)
    exit(0);
  }
 
endgame::~endgame() {
  delete[] d;  
  }

unsigned long endgame::index(int s, int t, position *p) {
  register int i,k,r;
  if (s) {
    r = 0; 
    k = p->a[PITS+1];
    for (i=PITS+1;i<LPIT-1;) {
      r += ci[i][k];
      k += p->a[++i];
      }
    r = r * bi[t-k] + di[t][k];
    k = 0;
    for (i=0;i<PITS-1;i++) {
      k += p->a[i];
      r += ci[i][k];
      }
    return r; 
    }
  else {
    r = 0;
    k = p->a[0];
    for (i=0;i<PITS-1;) {
      r += ci[i][k];
      k += p->a[++i];
      }
    r = r * bi[t-k] + di[t][k];
    k = 0;
    for (i=PITS+1;i<LPIT-1;i++) {
      k += p->a[i];
      r += ci[i][k];
      }
    return r;
    }
  }
  


 
