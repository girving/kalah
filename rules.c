// Code for kalah rules
// Geoffrey Irving
// 4sep0

#include "rules.h"

void fill_pos(position *p, int n) {
  p->w = -1;
  memset(p->a,n,LPIT);
  p->a[PITS] = p->a[LPIT] = 0;
  }

/* excessively optimized kalah movement routine */

extern int move(position *ps, register int i) {
  int s,t;
  register char *p,*sp,*ep;
  sp = ps->a;
  if (ps->s) {
    i += PITS+1;
    p = sp + i;
    s = *p + i;
    *p = 0;
    ep = sp+TPITS;
    t = sp[PITS];
    do {
      if (++p == ep)
        p = sp;
      (*p)++;
      } while (++i<s);
    s = sp[PITS] - t;
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
    ps->w = (s>i) ? 0 : ((s==i) ? 2 : 1);
    return 1;
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
    ps->w = (i>s) ? 0 : ((i==s) ? 2 : 1);
    return 1;
    }

  if (!i)
    ps->s = 1 - ps->s;
  return i;
  }

void write_pq(FILE *f, position p) {
  int i;
  for (i=0;i<=PITS;i++)
    fprintf(f,"%d ",bin(&p,i)); 
  for (i=0;i<=PITS;i++)
    fprintf(f,"%d ",o_bin(&p,i)); 
  putc('\n',f);
  }

void write_p(FILE *f, position p, int s) {
  int i;
  fprintf(f,"  ");
  for (i=0;i<PITS;i++)
    fprintf(f,"%3d",a_bin(&p,1-s,PITS-1-i));
  fprintf(f,"\n%2d",a_bin(&p,1-s,PITS));
  for (i=0;i<PITS;i++)
    fputs("   ",f);
  fprintf(f,"%3d\n  ",a_bin(&p,s,PITS));
  for (i=0;i<PITS;i++)
    fprintf(f,"%3d",a_bin(&p,s,i));
  putc('\n',f);
  }
