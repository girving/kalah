// Endgame database generator code
// Geoffrey Irving
// 08sept99

#include <stdio.h>
#include <unistd.h>
#include "params.h"
#include "rules.h"
#include "endgame.h"

long long si[100], ai[100]; 
long stones, crunched, saved;
endgame *eg;
 
void create() {
  FILE* f = fopen("endgame.dat","w");
  int i = 0;
  long l = 0;
  fwrite(&i,sizeof(int),1,f);
  fwrite(&l,sizeof(long),1,f);
  fclose(f);
  }

void initsize() {
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

void setup() {
  int i;
  FILE *f;
  if (access("endgame.dat",F_OK))
    create();
  f = fopen("endgame.dat","r");  
  eg = (endgame*) malloc(sizeof(endgame));
  read_endgame(eg,f,40);
  fclose(f);
  stones = saved = eg->n;
  crunched = eg->size * 2;
  initsize();
  }

void save() {
  FILE *f;
  if (stones == saved)
    return;
  f = fopen("endgame.dat","w");
  write_endgame(eg,f); 
  fclose(f);
  }

void info(int v) {
  int i,j,s;
  int h[32];
  if (!v) {
    for (i=0;i<32;i++)
      h[i] = 0;
    for (i=1;i<=eg->n;i++)
      for (j=eg->di[i][1];j<eg->di[i][i];j++)
        h[eg_getd(eg,j)]++;

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
  if (s > stones) {
    printf("Too many total stones.\n");
    return;
    }
  printf("Index: %ld\n",eg_index(eg,s,&p));
  printf("Entry: %d\n",eg_getd(eg,eg_index(eg,s,&p)));
  i = eg_lookup(eg,s,&p,&k);
  if (k == EG_EXACT)
    printf("Rate:  %d\n",i);
  else
    printf("Rate:  %d+\n",i);
  }

int crunch(int n, unsigned long l, position *p) {
  int i,s,v,r,k,e;
  position t;
  v = 1;
  p->a[LPIT] = p->a[PITS] = 0;  
  for (i=0;i<PITS;i++)
    if (bin(p,i)) {
      t = *p;    
      s = p->s;
      e = move(&t,i); 
      r = a_bin(p,s,PITS);
      if (t.w == -1) {
        k = eg_index(eg,n-r,&t);
        s = eg_getd(eg,k);
        if (s == 15) {
          s = crunch(n-r,k,&t);
          r = e ? r + s : n - s;
          }
        else
          r = e ? r + s + 1 : n - s - 1;
        }
      if (r > v)
        v = r;
      }
  eg_setd(eg,l,v);
  crunched++;
  return v;
  }

void complete(int n) {
  position p;            
  FILE* f;
  int i,j,k;

  p.s = 0;
  fill_pos(&p,0);

  if (n <= eg->n) 
    return;
  free(eg->d);
  eg->d = (unsigned char*) malloc(eg->ai[n][n] >> 1);
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
          k = eg_index(eg,i,&p);
          if (eg_getd(eg,k) == 15)
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
  eg->n = stones = n;
  }
          
int main() {
  char c[10] = "h";
  int v;

  setup();
  
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

      case 'i': info(0); 
                break;

      case 't': scanf("%d",&v);
                info(v);
                break;
 
      case 'r': test();
                break;

      case 'c': scanf("%d",&v);
                if (v > stones) {
                  save();
                  complete(v);
                  }
                else
                  printf("Already computed.\n");
                break;

      case 's': save();
                break;

      case 'q': 
      case 'e': save();
                free_endgame(eg);
                return 0;
      }
    printf("> ");
    scanf("%s",&c);
    }
  }
       









