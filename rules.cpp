// Code for mancala rules
// Geoffrey Irving
// 3/24/98

#include "rules.h"

position::position(int n) {
  w = -1;
  memset(a,n,LPIT);
  a[PITS] = a[LPIT] = 0;
  }

/* excessively optimized mancala movement routine */

int position::move(register int side, register int i) {
  register int s;
  register char *p,*sp,*ep;
  sp = a;
  if (side) {
    i += PITS+1;
    p = sp + i;
    s = *p + i;
    *p = 0;
    ep = sp+TPITS;
    side = sp[PITS];
    do {
      if (++p == ep)
        p = sp;
      (*p)++;
      } while (++i<s);
    s = sp[PITS] - side;
    if (s) { 
      sp[PITS] -= s;
      do {
        if (++p == ep)
          p = sp;
        else if (p == sp + PITS)
          p++;
        (*p)++;
        } while (--s);
      }
    s = p - sp;
    ep--;
    if (s>PITS && *p==1 && s<LPIT) {
      *ep += sp[LPIT-1-s];
      (*ep)++;
      sp[LPIT-1-s] = 0;
      *p = 0;
      i = 0;
      }
    else
      i = (p == ep);
    }
  else {
    p = sp + i;
    s = *p + i;
    *p = 0;
    ep = sp + LPIT;
    do {
      if (++p == ep)
        p = sp;
      (*p)++;
      } while (++i<s);
    s = p - sp;
    ep = sp + PITS;
    if (s<PITS && *p==1) {
      *ep += sp[LPIT-1-s];
      (*ep)++;
      sp[LPIT-1-s] = 0;
      *p = 0;
      i = 0;
      }
    else
      i = (p == ep);
    }

#if PITS == 2 
#define EMPTY_PIT_CHECK *++p
#elif PITS == 3
#define EMPTY_PIT_CHECK *++p || *++p
#elif PITS == 4
#define EMPTY_PIT_CHECK *++p || *++p || *++p
#elif PITS == 5
#define EMPTY_PIT_CHECK *++p || *++p || *++p || *++p
#elif PITS == 6
#define EMPTY_PIT_CHECK *++p || *++p || *++p || *++p || *++p
#endif

  p = sp;
  if (!(*p || EMPTY_PIT_CHECK)) { 
    ep = sp + LPIT;
    i = *ep;
    p++;
    for (s=0;s<PITS;s++) {
      i += *++p; 
      *p = 0; 
      }
    *ep = i;
    s = *(sp+PITS);
    w = (s>i) ? 0 : ((s==i) ? 2 : 1);
    return 0;
    }
  
  p = sp + PITS+1;
  if (!(*p || EMPTY_PIT_CHECK)) { 
    ep = sp + PITS;
    p = sp;
    i = *ep;
    for (s=1;s<PITS;s++) {
      i += *p; 
      *p++ = 0; 
      }
    i += *p; *p = 0;
    *ep = i;
    s = *(sp+LPIT);
    w = (i>s) ? 0 : ((i==s) ? 2 : 1);
    return 0;
    }

  return i;
  }

void printq(FILE *f, int s, position p) {
  int i;
  for (i=0;i<=PITS;i++)
    fprintf(f,"%d ",p.bin(s,i)); 
  for (i=0;i<=PITS;i++)
    fprintf(f,"%d ",p.bin(1-s,i)); 
  putc('\n',f);
  }

void print(FILE *f, int s, position p) {
  int i;
  fprintf(f,"  ");
  for (i=0;i<PITS;i++)
    fprintf(f,"%3d",p.bin(1-s,PITS-1-i));
  fprintf(f,"\n%2d",p.bin(1-s,PITS));
  for (i=0;i<3*PITS;i++)
    putc(' ',f);
  fprintf(f,"%3d",p.bin(s,PITS));
  for (i=0;i<PITS;i++)
    fprintf(f,"%3d",p.bin(s,i));
  putc('\n',f);
  }
