// Endgame database generator code
// Geoffrey Irving
// 08sept99

#include <stdio.h>
#include <unistd.h>
#include "params.h"
#include "rules.h"
#include "endgame.h"

class generator {
    long long si[100], ai[100]; 
 
    void create();
    void initsize();
    int crunch(int s, int n, unsigned long l, position& p);
  public:
    long stones, crunched, saved;
    endgame* eg;
    
    generator();
    void save();
    void info(int v);
    void test();
    void complete(int n);
  };

void generator::create() {
  FILE* f = fopen("endgame.dat","w");
  int i = 0;
  long l = 0;
  fwrite(&i,sizeof(int),1,f);
  fwrite(&l,sizeof(long),1,f);
  fclose(f);
  }

void generator::initsize() {
  int i,j;
  long long ci[100];
  ci[0] = 1;
  for (j=1;j<100;j++)
    ci[j] = ci[j-1] * (j+PITS-1) / j;
        
  si[0] = si[1] = 0;
  ai[0] = ai[1] = 0;
  for (i=2;i<100;i++) {
    si[i] = 0; 
    for (j=1;j<i;j++)
      si[i] += ci[j] * ci[i-j];
    if (si[i] & 1)
      si[i]++;
    si[i] >>= 1;
    ai[i] = ai[i-1] + si[i];
    }
  }

generator::generator() {
  int i,nm[40];
  if (access("endgame.dat",F_OK))
    create();
  for (i=0;i<40;i++)
    nm[i] = 1;
  FILE* f = fopen("endgame.dat","r");  
  eg = new endgame(f,40,nm);
  fclose(f);
  stones = saved = eg->n;
  crunched = eg->size * 2;
  initsize();
  }

void generator::save() {
  if (stones == saved)
    return;
  FILE* f = fopen("endgame.dat","w");
  eg->save(f);
  fclose(f);
  }

void generator::info(int v) {
  int i,j,s;
  int h[32];
  if (!v) {
    for (i=0;i<32;i++)
      h[i] = 0;
    for (i=1;i<=eg->n;i++)
      for (j=eg->di[i][1];j<eg->di[i][i];j++)
        h[eg->getd(j)]++;

    printf("Database info:\n");
    printf("  Stones: %ld\n",stones);
    printf("  Size:   %ld\n",eg->size);
    printf("  Done:   %ld\n",crunched);

    printf("Frequency:\n");
    for (i=0;i<32;i++)
      if (h[i])
        printf("  %2d) %d\n",i,h[i]);
    putchar('\n');
    }
  else {
    printf("Table sizes:\n");
    for (i=2;i<=v;i++)
      printf("  %2d) %9lld, %9lld\n",i,si[i],ai[i]);
    putchar('\n');
    }
  }

void generator::test() {
  position p;
  int i,k,s=0;
  printf("Enter position: ");
  for (i=0;i<TPITS;i++) {
    scanf("%d",&k);
    p.a[i] = k; 
    s += k;
    }
  s -= p.a[PITS] + p.a[LPIT];
  if (s > stones) {
    printf("Too many total stones.\n");
    return;
    }
  printf("Index: %ld\n",eg->index(0,s,&p));
  printf("Entry: %d\n",eg->getd(eg->index(0,s,&p)));
  i = eg->lookup(0,s,&p,&k);
  if (k == EG_EXACT)
    printf("Rate:  %d\n",i);
  else
    printf("Rate:  %d+\n",i);
  }

int generator::crunch(int s, int n, unsigned long l, position& p) {
  int i,v,r,k;
  position t;
  v = 1;
  p.a[LPIT] = p.a[PITS] = 0;  
  for (i=0;i<PITS;i++)
    if (p.bin(s,i)) {
      t = p;    
      k = t.move(s,i); 
      r = t.bin(s,PITS);
      if (t.w == -1) {
        if (k) {
          k = eg->index(s,n-r,&t);
          if (eg->getd(k) == 15)
            r += crunch(s,n-r,k,t); 
          else 
            r += eg->getd(eg->index(s,n-r,&t)) + 1;
          }
        else {
          k = eg->index(1-s,n-r,&t);
          if (eg->getd(k) == 15)
            r = n - crunch(1-s,n-r,k,t); 
          else 
            r = n - eg->getd(k) - 1;
          }
        }
      if (r > v)
        v = r;
      }
  eg->setd(l,v);
  crunched++;
  return v;
  }

void generator::complete(int n) {
  position p(0);            
  FILE* f;
  int i,j,k;

  if (n <= eg->n) 
    return;
  delete[] eg->d;
  eg->d = new unsigned char[eg->ai[n][n] >> 1];
  f = fopen("endgame.dat","r");  
  fseek(f,sizeof(int)+sizeof(long),SEEK_CUR);
  fread(eg->d,sizeof(char),eg->size,f);
  fclose(f);
  for (i=eg->size;i<eg->ai[n][n] >> 1;i++)
    eg->d[i] = 255;
  printf("s = %ld, a = %lld\n",eg->size,eg->ai[n][n]); 
  eg->size = eg->ai[n][n] >> 1;

  for (i=eg->n+1;i<=n;i++) {
    printf("%d) ",i);
    fflush(stdout);
    for (j=1;j<i;j++) {
      p.a[PITS-1] = j;  
      for (;;) {
        p.a[LPIT-1] = i-j;
        for (;;) {
          k = eg->index(0,i,&p);
          if (eg->getd(k) == 15)
            crunch(0,i,k,p);
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
  eg->n = stones = n;
  }
          
void main() {
  generator g;          
  char c[10];
  int v;
  int unsaved;
  
  *c = 'h';;
  unsaved = 0;
 
  for (;;) {
    switch (*c) {
      case 'h': printf("Commands:\n");
                printf("  h   - help\n");
                printf("  i   - database info\n");
                printf("  t n - size table to n stones\n");
                printf("  r   - lookup a position's rate\n");
                printf("  c n - complete table to n stones\n"); 
                printf("  s   - save database\n");
                printf("  e   - exit\n\n");
                break;

      case 'i': g.info(0); 
                break;

      case 't': scanf("%d",&v);
                g.info(v);
                break;
 
      case 'r': g.test();
                break;

      case 'c': scanf("%d",&v);
                if (v > g.stones) {
                  g.save();
                  g.complete(v);
                  }
                else
                  printf("Already computed.\n");
                break;

      case 's': g.save();
                break;

      case 'q': 
      case 'e': g.save();
                return 0;
      }
    printf("> ");
    scanf("%s",&c);
    }
  }
       









