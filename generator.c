// Endgame database generator code
// Geoffrey Irving
// 08sept99

#include <stdio.h>
#include <unistd.h>
#include "params.h"
#include "rules.h"
#include "endgame.h"

char *file = "endgame.dat";
long long ai[50][50],bi[50],ci[LPIT][50]; 
int stones, saved, maxval;
long long size, crunched;
endgame *eg;
 
void create() {
  if (access(file,F_OK)) {
    int b;
    printf("Creating null endgame database:\n");
    printf("  Enter bits per element: ");
    scanf("%d",&b);
    eg_create(file,b);
    }
  }

void load(int n) {
  FILE *f;
  endgame_header h;
  if (n > stones) {
    printf("Loading endgame database to %d stones...",n);
    fflush(stdout);
    if (stones >= 0)
      free_endgame(eg);
    f = fopen(file,"r");
    fread(&h,sizeof(endgame_header),1,f);
    fseek(f,0,SEEK_SET);
    read_endgame(eg,f,n);
    fclose(f);
    saved = h.n;
    size = h.size;
    stones = eg->n;
    maxval = eg_maxval(eg);
    printf("done\n");
    }
  }

void save() {
  if (stones > saved) {
    FILE *f = fopen(file,"w");
    printf("Saving endgame database...");
    fflush(stdout);
    write_endgame(eg,f); 
    fclose(f);
    saved = stones;
    printf("done\n");
    }
  }

void info(int v) {
  int i,j,s;
  if (!v) {
    printf("Database info:\n");
    printf("  Stones:  %d\n",saved);
    printf("  Loaded:  %d\n",stones);
    printf("  Bits:    %d\n",eg->bits);
    printf("  Maxval:  %d\n",maxval);
    printf("  Size:    %ld\n",size);
    printf("  Entries: %ld\n",size * 8 / eg->bits);
    printf("  Nodes:   %lld\n",crunched);
    putchar('\n');
    }
  else if (v > 40)
    printf("Too many stones\n");
  else {
    long long a1,a2;
    printf("Table sizes:\n");
    for (i=2;i<=v;i++) {
      a1 = ai[i][i] >> 3;
      a2 = ai[i][1] >> 3;
      printf("  %2d) %10lld, %10lld  /  %10lld, %10lld\n",i,(a1-a2)*4,a1*4,
                                                            (a1-a2)*5,a1*5);
      }
    putchar('\n');
    }
  }

void test() {
  position p;
  int i,k,s=0;
  p.s = 0;
  printf("Enter position: ");
  for (i=0;i<TPITS;i++) {
    scanf("%d",&k);
    p.a[i] = k; 
    s += k;
    }
  s -= p.a[PITS] + p.a[LPIT];
  if (s > saved && s > stones) {
    printf("Too many total stones: %d > %d\n",s,saved > stones ? saved : stones);
    return;
    }
  load(s);
  printf("Index: %ld\n",eg_index(eg,s,&p));
  printf("Entry: %d\n",eg_getd(eg,eg_index(eg,s,&p)));
  i = eg_lookup(eg,s,&p,&k);
  if (k == EG_EXACT)
    printf("Rate:  %d\n",i);
  else
    printf("Rate:  %d+\n",i);
  }

int crunch(int n, size_t l, position *p) {
  int i,s,v,r,e,cn;
  size_t k;
  position t;
  v = 1;
  p->a[LPIT] = p->a[PITS] = 0;
  for (i=0;i<PITS;i++)
    if (bin(p,i)) {
      t = *p;    
      s = p->s;
      e = move(&t,i); 
      r = a_bin(&t,s,PITS);
      if (t.w < 0) {
        cn = n - t.a[PITS] - t.a[LPIT];
        k = eg_index(eg,cn,&t);
        s = eg_getd(eg,k);
        if (s == maxval) {
          s = crunch(cn,k,&t);
          r = e ? r + s : n - s;
          }
        else
          r = e ? r + s + 1 : n - s - 1;
        }
      if (v < r)
        v = r;
      }
  r = v-1;
  if (r > maxval) r = maxval;
  eg_setd(eg,l,r);
  crunched++;
  return v;
  }

void complete(int n) {
  position p;            
  long long temp;
  int i,j;
  size_t k;
  if (n <= saved) {
    printf("Already computed\n");
    return;
    }
  p.s = 0;
  fill_pos(&p,0);

  save();
  load(saved);
  size = eg->ai[n][n] * eg->bits / 8;
  printf("Allocating memory for %d stones...",n);
  fflush(stdout);
  eg->d = realloc(eg->d,size);
  memset(eg->d+eg->size, 255, size - eg->size);
  printf("done\n");
  eg->n = n;
  eg->size = size;

  for (i=stones+1;i<=n;i++) {
    printf("%2d) ",i);
    fflush(stdout);
    for (j=1;j<i;j++) {
      p.a[PITS-1] = j;  
      for (;;) {
        p.a[LPIT-1] = i-j;
        for (;;) {
          k = eg_index(eg,i,&p);
          temp++;
          if (eg_getd(eg,k) == maxval)
            crunch(i,k,&p);
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
  stones = n;
  }
          
int main(int argc, char **argv) {
  char c[10] = "h";
  int v;
  eg = malloc(sizeof(endgame));
  eg_init_long_tables(ai,bi,ci);
  crunched = 0;
  stones = saved = -1;
  if (argc == 2)
    file = argv[1];
  create();
  load(0);

  for (;;) {
    if (index("tc",*c))
      scanf("%d",&v);
    switch (*c) {
      case 'h': printf("Commands:\n");
                printf("  h   - help\n");
                printf("  i   - database info\n");
                printf("  t n - size table to n stones\n");
                printf("  r   - lookup a position's rate\n");
                printf("  c n - complete table to n stones\n"); 
                printf("  s   - save database\n");
                printf("  q   - exit\n\n");
                break;

      case 'i': info(0); break;
      case 'r': test(); break;
      case 's': save(); break;
      case 't': info(v); break;
      case 'c': complete(v); break;

      case 'q':
      case 'e': save();
                free_endgame(eg);
                return 0;

      default: printf("Invalid command\n"); 
      }
    printf("> ");
    scanf("%s",&c);
    }
  }
       









