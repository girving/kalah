// Kalah minimax search code
// Geoffrey Irving
// 1sep0

#include "crunch.h"

hash* ha = 0;    
endgame* eg = 0; 

int stones = 0; 
int calldepth;
double start;
stat_t stat;

void init_stat() {
  stat.maxdepth = 0;
  stat.elapsed = 0;
  stat.nodes = stat.peeks = stat.cutoffs = stat.ha_lookups = 0;
  stat.eg_lookups = stat.f_cutoffs = 0;
  }

void format_elapsed(char *s, long t) {
  div_t d = div(t,86400);
  div_t h = div(d.rem,3600);
  div_t m = div(h.rem,60);
  if (d.quot)
    s += sprintf(s,"%d days, ",d.quot);
  if (d.quot || h.quot)
    sprintf(s,"%d:%.2d:%.2d",h.quot,m.quot,m.rem);
  else if (m.quot)
    sprintf(s,"%d:%.2d",m.quot,m.rem);
  else
    sprintf(s,"%d",m.rem);
  }

void print_stat(FILE *f, char *p) {
  char t[50];
  format_elapsed(t,(long)stat.elapsed);
  fprintf(f,"%sThinking time:    %s\n",p,t);
  fprintf(f,"%sMaximum depth:    %d\n",p,stat.maxdepth);
  fprintf(f,"%sNodes searched:   %lld\n",p,stat.nodes);
  fprintf(f,"%sNodes peeked:     %lld\n",p,stat.peeks);
  fprintf(f,"%sBeta cutoffs:     %lld\n",p,stat.cutoffs);
  fprintf(f,"%sHash cutoffs:     %lld\n",p,stat.ha_lookups);
  fprintf(f,"%sFutility cutoffs: %lld\n",p,stat.f_cutoffs);
  fprintf(f,"%sEndgame lookups:  %lld\n",p,stat.eg_lookups);
  fprintf(f,"%sNodes per second: %.lf\n",p,stat.nodes/stat.elapsed);
  }

inline void smallsort(int n, int* r, int* v) {
  register int i,j,tr,tv;
  for (i=1;i<n;i++)
    if (r[i-1] < r[i]) {
      tr = r[i];
      tv = v[i];
      j = i;
      do {
        r[j] = r[j-1];
        v[j] = v[j-1];
        j--;
        } while (j > 0 && r[j-1] < tr);
      r[j] = tr;
      v[j] = tv;
      }
  } 

/* Handle leaf nodes, endgame lookups, futility pruning, and hash lookups */
inline int peek(int s, position *p, datumaccess *da, int d, int *rd, int a) {
  int r,k,t;

  STAT(stat.peeks++);

  /* handle end of game */
  if (p->w >= 0) {
    *rd = 200;
    return p->rate(s);
    }
  
  k = stones - p->a[LPIT] - p->a[PITS];
 
  /* endgame database */
  if (eg->known(k)) {
    STAT(stat.eg_lookups++);
    r = eg->lookup(s,k,p,&t);
    if (t == EG_EXACT || r > a) { 
      *rd = 250; 
      return r;
      }
    }
  
  r = p->rate(s);

  /* futility pruning */
  if (r + k <= a) {
    STAT(stat.f_cutoffs++);
    *rd = 250;
    return r + k;
    }
  if (r - k + 2 > a) {
    STAT(stat.f_cutoffs++);
    *rd = 250;
    return r - k + 2;
    }

  /* transposition table */ 
  ha->lookup(da,s,p);
  if (da->gett() && da->getd() >= d &&
         (da->gett() == DV_LO && da->getr() > a ||
          da->gett() == DV_HI && da->getr() <= a)) {
    STAT(stat.ha_lookups++);
    *rd = da->getd();
    return da->getr();
    }

  /* evaluate leaf nodes */
  if (!d) {
    *rd = 0;
    return r;
    }  

  /* failure */
  *rd = -1;      
  return r;
  }

/* Minimal-window fail-soft jamboree negamax search */
int crunch(int s, char *m, position *p, datumaccess *da, int d, int *rd, int a) {
  int i,r,v,n,cd,tm;

  int o[PITS];
  int cr[PITS];
  int k[PITS];
  char cm[MAXMVC];
  position t[PITS];
  datumaccess ca[PITS];

  STAT(stat.nodes++);
  STAT(if (stat.maxdepth < calldepth - d) stat.maxdepth = calldepth - d);

  /* peek at all children and construct move ordering */
  n = 0;
  d--;
  *rd = 250;
  v = -1000;
  tm = da->gett() ? da->getm() : -1;
  for (i=PITS-1;i>=0;i--)      
    if (p->bin(s,i)) {
      t[i] = *p;
      k[i] = t[i].move(s,i);
      r = k[i] ? peek(s,&t[i],&ca[i],d,&cd,a) : -peek(1-s,&t[i],&ca[i],d,&cd,-a-1);
      if (++cd > 0) {    // peek sucessful
        if (cd < *rd)
          *rd = cd;
        if (v < r) {
          v = r;
          *m = i + '0'; 
          m[1] = 0;
          if (v > a) {
            STAT(stat.cutoffs++);
            n = 0; 
            break;
            }
          }
        }
      else {            // peek failed, do some move ordering
        o[n] = i;       
        if (i == tm)      // isn't this move ordering beautifully simple?
          cr[n] = 1000;
        else if (k[i])
          cr[n] = 200 + i; 
        else 
          cr[n] = r;
        n++;
        }
      }

  smallsort(n,cr,o);   // complete move ordering

  /* full child searches */
  for (i=0;i<n;i++) {
    r = k[o[i]] ?  crunch(  s,cm,&t[o[i]],&ca[o[i]],d,&cd,a) 
                : -crunch(1-s,cm,&t[o[i]],&ca[o[i]],d,&cd,-a-1); 
    if (++cd < *rd)
      *rd = cd;
    if (v < r) {
      v = r;
      *m = o[i] + '0';
      if (k[o[i]])
        strcpy(m+1,cm);
      else
        m[1] = 0;
      if (v > a) {
        STAT(stat.cutoffs++);
        break;
        }
      }
    }
  
  /* store search results in transposition table */
  if (*rd > LOWDEPTH && (!da->gett() || *rd >= da->getd()) 
                     && da->seta(*rd,*m-'0',v))
    da->sett(v > a ? DV_LO : DV_HI);

  return v;
  }

/* Top level minimal-window fail-soft alpha-beta negamax search */
int toplevel(int s, char *m, position *p, int d, int *rd, int a) {
  datumaccess da;

  STAT(calldepth = d);

  /* transposition table check */
  ha->lookup(&da,s,p);
  if (da.gett() && da.getd() >= d &&
         (da.gett() == DV_LO && da.getr() > a ||
          da.gett() == DV_HI && da.getr() <= a)) {
    STAT(stat.ha_lookups++);
    *rd = da.getd();
    *m = da.getm() + '0';
    m[1] = 0;
    return da.getr();
    }

  /* proceed with full search */
  return crunch(s,m,p,&da,d,rd,a);
  }

int mtdf(int s, char *m, position *p, int d, int *rd, int g, int f) {
  int a, td;
  char cm[MAXMVC];
  int r = g;
  int lo = -1000;
  int hi = 1000;
  *rd = 250;
  if (f & SF_VERBOSE) 
    printf("MTD(f): d %d, g %d, t %.f\n",d,g,time(NULL)-start);

  do {
    a = (r == lo) ? r : r-1;
    r = toplevel(s,cm,p,d,&td,a);
     if (td < *rd)
       *rd = td;
    if (r > a) {
      lo = r;
      strcpy(m,cm);
      }
    else
      hi = r;
    if (f & SF_VERBOSE) {
      printf("  %d %d, a %d, d %d, m %s, t %.f\n",lo,hi,a,td,cm,time(NULL)-start);
      if (f & SF_FLOOD && (d > 15 || f & SF_FULL)) {
        stat.elapsed = time(NULL) - start;
        print_stat(stdout,"  ");
        }
      }
    if (f & SF_FULL && f & SF_SINGLE)
      break;
    } while (lo < hi);

  if (f & SF_VERBOSE) putchar('\n');
  return r;
  }

int solve(int s, char *m, position *p, int d, int *rd, int g, int f) {
  int i,r,jump;
  start = time(NULL);

  /* count total stones */
  stones = 0;
  for (i=0;i<TPITS;i++)
    stones += p->a[i];

  r = (f & SF_GUESS) ? g : p->rate(s);
  jump = (f & SF_JUMP) ? 0 : JUMPDEPTH;
  if (f & SF_FLOOD) f |= SF_VERBOSE;
  *rd = 0;

  i = d % STEPSIZE + STEPSIZE;
  for (;*rd<d;i+=STEPSIZE) {  
    if (i > jump)
      i = d;
    if (i == d)
      f |= SF_FULL;

    r = mtdf(s,m,p,i,rd,r,f);

    if (*rd > i) {
      if (*rd > d) break;
      i = *rd - (*rd-d+100*STEPSIZE) % STEPSIZE;
      }
    }

  stat.elapsed += time(NULL) - start;
  if (f & SF_STALE) ha->setstale();

  return r;
  }


/* Miscellaneous functions */

void init_hash(int size, char *file) {
  if (!file || access(file,F_OK)) {  // hash file doesn't exist
    ha = new hash(size);
    }
  else {
    FILE *f = fopen(file,"r");
    ha = new hash(f);
    fclose(f);
    }
  } 

void save_hash(char *file) {
  if (file) {
    FILE *f = fopen(file,"w");
    ha->save(f);
    fclose(f);
    }
  }

void close_hash() {
  delete ha;
  }

void init_endgame(int size, char *file) {
  if (!access(file,F_OK)) {          // endgame database exists
    FILE *f = fopen(file,"r");
    eg = new endgame(f,size);
    fclose(f);
    }
  else {
    fprintf(stderr,"Cannot open endgame database\n");
    exit(-1);
    }
  }

void close_endgame() {
  delete eg;
  }
