// Kalah transposition table hash code
// Geoffrey Irving
// 4sep0

// Hash function written by Bob Jenkins

#include "hash.h"

#define HASHFILECHECK 27318

void create_hash(hash *h, int bitsize) {
  long i;
  h->bits = bitsize;
  h->size = hashsize(h->bits);
  h->mask = hashmask(h->bits);
  h->entries = 0;
  h->d = (datum*) malloc(h->size * sizeof(datum));
  for (i=0;i<h->size;i++)
    h->d[i].f = 0;
  }

void read_hash(hash *h, FILE *f) {
  int t;
  if (fread(&t,sizeof(int),1,f) != 1 || t != HASHFILECHECK) {
    fprintf(stderr,"Corrupt hash file\n");
    exit(-1);
    }

  fread(&h->bits,sizeof(int),1,f);
  fread(&h->size,sizeof(long),1,f);
  fread(&h->entries,sizeof(long),1,f);
  fread(&h->mask,sizeof(long),1,f);

  h->d = (datum*) malloc(h->size * sizeof(datum));
  if (fread(h->d,sizeof(datum),h->size,f) != h->size)
    exit(-1);
  }

void write_hash(hash *h, FILE *f) {
  int t = HASHFILECHECK;
  fwrite(&t,sizeof(int),1,f);
  fwrite(&h->bits,sizeof(int),1,f);
  fwrite(&h->size,sizeof(long),1,f);
  fwrite(&h->entries,sizeof(long),1,f);
  fwrite(&h->mask,sizeof(long),1,f);

  if (fwrite(h->d,sizeof(datum),h->size,f) != h->size)
    exit(0); 
  }

void free_hash(hash *h) {
  free(h->d);
  }

extern void setstale(hash *h) {
  register datum *p, *e;
  e = h->d + h->size;
  for (p=h->d;p<e;p++)
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

void ha_lookup(hash *h, datumaccess *da, position *p) {
  register ub4 a,b,k;

  if (p->s) {
    a = packpits(p->a);
    b = packpits(p->a+PITS+1);
    }
  else {
    b = packpits(p->a);
    a = packpits(p->a+PITS+1);
    }

  if (a == HASHOVERFLOW || b == HASHOVERFLOW) {
    da->ko = 1;
    da->t = DV_INVALID;
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
  k &= h->mask;

  da->c = h->d + k; 
  da->br = rate(p);
  da_verify(da);
  }

int da_seta(datumaccess *da, int d, int m, int r) {
  if (da->ko) return 0;
  if (da->c->b1 == da->b1 && da->c->b2 == da->b2) {
    da->c->d = d;
    da->c->m = m;
    da->c->r = r - da->br;
    if (!da->t) {
      da->c->b1 = da->b1;
      da->c->b2 = da->b2;
      }
    return 1;
    }
  else if ((da->c->f & D_STALE) || da->c->d <= d) {
    da->c->d = d; 
    da->c->m = m; 
    da->c->r = r - da->br;
    da->c->b1 = da->b1;
    da->c->b2 = da->b2;
    return 1;
    }
  return 0;
  }


