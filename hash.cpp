// Mancala transposition table hash code
// Geoffrey Irving
// 29july99

// Hash function written by Bob Jenkins

#include "hash.h"

#define HASHFILECHECK 27318

hash::hash(int bitsize) {
  long i;
  bits = bitsize;
  size = hashsize(bits);
  mask = hashmask(bits);
  entries = 0;
  d = new datum[size];
  for (i=0;i<size;i++)
    d[i].f = 0;
  }

hash::hash(FILE* f) {
  int t;
  if (fread(&t,sizeof(int),1,f) != 1 || t != HASHFILECHECK)
    exit(0);
  fread(&bits,sizeof(int),1,f);
  fread(&size,sizeof(long),1,f);
  fread(&entries,sizeof(long),1,f);
  fread(&mask,sizeof(long),1,f);

  d = new datum[size];
  if (fread(d,sizeof(datum),size,f) != size)
    exit(0); 
  }

void hash::save(FILE* f) {
  int t = HASHFILECHECK;
  fwrite(&t,sizeof(int),1,f);
  fwrite(&bits,sizeof(int),1,f);
  fwrite(&size,sizeof(long),1,f);
  fwrite(&entries,sizeof(long),1,f);
  fwrite(&mask,sizeof(long),1,f);

  if (fwrite(d,sizeof(datum),size,f) != size)
    exit(0); 
  }

hash::~hash() {
  delete[] d;
  }

void hash::setstale() {
  register datum *p, *e;
  e = d + size;
  for (p=d;p<e;p++)
    if (p->f)
      p->f |= D_STALE; 
  }

#define HASHOVERFLOW (1<<30)

inline ub4 packpits(char* a) {

#if PITS == 2  
#define PIT_COMBINE *++a
#elif PITS == 3
#define PIT_COMBINE *++a | *++a
#elif PITS == 4
#define PIT_COMBINE *++a | *++a | *++a
#elif PITS == 5
#define PIT_COMBINE *++a | *++a | *++a | *++a
#elif PITS == 6
#define PIT_COMBINE *++a | *++a | *++a | *++a | *++a
#endif

  if ((*a | PIT_COMBINE) & ~31) 
    return HASHOVERFLOW;
  a -= PITS-1;

  return a[0] | (ub4)a[1]<<5 
#if PITS > 2
              | (ub4)a[2]<<10 
#if PITS > 3
              | (ub4)a[3]<<15
#if PITS > 4
              | (ub4)a[4]<<20 
#if PITS > 5
              | (ub4)a[5]<<25 
#endif
#endif
#endif
#endif
            ;
  }

void hash::lookup(datumaccess *da, int s, position *p) {
  register ub4 a,b,k;

  if (s) {
    a = packpits(p->a);
    b = packpits(p->a+PITS+1);
    }
  else {
    b = packpits(p->a);
    a = packpits(p->a+PITS+1);
    }

  if (a == HASHOVERFLOW || b == HASHOVERFLOW) {
    da->ko = 1;
    da->cf = DV_INVALID;
    return;
    }

  da->ko = 0;
  da->b1 = a;
  da->b2 = b;

  a += 0x9e3779b9; 
  b += 0x9e3779b9; 
  k = 0xfca09587;
  a -= b; a -= k; a ^= (k>>13); 
  b -= k; b -= a; b ^= (a<<8); 
  k -= a; k -= b; k ^= (b>>13); 
  a -= b; a -= k; a ^= (k>>12);  
  b -= k; b -= a; b ^= (a<<16); 
  k -= a; k -= b; k ^= (b>>5); 
  a -= b; a -= k; a ^= (k>>3);  
  b -= k; b -= a; b ^= (a<<10);
  k -= a; k -= b; k ^= (b>>15);
  k &= mask;

  da->c = d + k; 
  da->br = p->rate(s);
  da->verify();
  }

int datumaccess::seta(int d, int m, int r) {
  if (ko) return 0;
  if (c->b1 == b1 && c->b2 == b2) {
    c->d = d;
    c->m = m;
    c->r = r - br;
    if (!cf) {
      c->b1 = b1;
      c->b2 = b2;
      }
    return 1;
    }
  else if ((c->f & D_STALE) || c->d <= d) {
    c->d = d; 
    c->m = m; 
    c->r = r - br;
    c->b1 = b1;
    c->b2 = b2;
    return 1;
    }
  return 0;
  }


