// Miscellaneous useful kalah operations 
// Geoffrey Irving
// 18aug00

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>

#include "rules.h"
#include "endgame.h"

void usage() {
  fprintf(stderr,"Usage: twiddle [OPTIONS] m n s\n");
  fprintf(stderr,"       twiddle [OPTIONS] r n\n");
  fprintf(stderr,"       twiddle [OPTIONS] d\n");
  fprintf(stderr,"Options:\n");
  fprintf(stderr,"  -h     display this help screen and quit\n");
  fprintf(stderr,"  -v     verbose output\n");
  fprintf(stderr,"  For command e:\n");
  fprintf(stderr,"    -t n   truncate to n stones (required)\n");
  fprintf(stderr,"    -b n   truncate to n bits\n");
  fprintf(stderr,"Commands:\n");
  fprintf(stderr,"  m p s  expand movelist s starting with position p\n");
  fprintf(stderr,"  r p    make trial moves starting with position p\n");
  fprintf(stderr,"  e s d  reduce endgame s and store in d\n");
  fprintf(stderr,"  d      display miscellaneous information\n");
  fprintf(stderr,"Notes:\n");
  fprintf(stderr,"  1) A single number can be given as a position to\n");
  fprintf(stderr,"     indicate a board filled with that number of stones\n");
  exit(0);
  }

void misc() {
  printf("Miscellaneous information:\n");
  printf("  0xffffffff: %d\n",0xffffffff);
  printf("  UINT_MAX: %d\n",UINT_MAX);
  printf("  ULONG_MAX: %ld\n",ULONG_MAX);
  }

void expandmovelist(position p, char *m, int verbose) {
  char em[300];
  char *q = em;
  int k;
  if (verbose) {
    printf("Positions:\n  ");
    write_pq(stdout,p);
    }
  while (*q++ = *m) {
    if (*m >= '0' && *m < PITS+'0') {
      k = move(&p,*m-'0'); 
      if (!k)
        *q++ = '-';
      if (verbose) {
        printf("  ");
        write_pq(stdout,p);
        }
      }
    else if (*m == '-')
      q--;
    else {
      fputs("Invalid move list\n",stderr);
      return;
      }
    m++;
    }
  printf("Expanded move list: %s\n",em);
  }

void trialmoves(position p, int verbose) {
  int m;
  while (p.w < 0) {
    if (verbose)
      write_p(stdout,p,0);
    else
      write_pq(stdout,p);
    printf("Side %d, enter move: ",p.s);
    scanf("%d",&m);
    if (m < 0 || m >= PITS)
      break;
    move(&p,m);
    }
  }

#define BLOCK 5120

void reduce_endgame(int n, int b, char *sfile, char *dfile) {
  FILE *s, *d;
  int i;
  endgame e,e2;
  endgame_header h,nh;
  unsigned char buf[BLOCK];
  int block;
  if (!n && !b) {
    fprintf(stderr,"Must supply either -t or -b\n");
    return;
    }
  if (b && (b < 4 || b > 5)) {
    fprintf(stderr,"Invalid number of bits\n");
    return;
    }
  s = fopen(sfile,"r");
  d = fopen(dfile,"w");
  if (!s || !d) {
    fprintf(stderr,"Failed to open files\n");
    return;
    }
  fread(&h,sizeof(endgame_header),1,s);
  if (h.n < n || h.bits < b) {
    fprintf(stderr,"Cannot create new endgame data\n");
    return;
    }

  eg_init_tables(&e);
  e.d = e2.d = buf;
  e.bits = h.bits;
  nh = h;
  if (b && h.bits > b)
    nh.bits = b;
  e2.bits = nh.bits;
  if (n && h.n > n) {
    nh.n = n;
    }
  nh.size = e.ai[n][n] * nh.bits / 8; 
  fwrite(&h,sizeof(endgame_header),1,d);
  
  while (h.size) {
    block = h.size > BLOCK ? BLOCK : h.size;
    h.size -= block;
    fread(buf,1,block,s);
    for (i=0;i<block*8/h.bits;i++)
      eg_setd(&e2,i,eg_getd(&e,i)); 
    fwrite(buf,1,block*nh.bits/h.bits,d);
    }
  fclose(s);
  fclose(d);
  }

void getposition(int argc, char **argv, position *p) {
  int i;
  p->s = 0;
  p->w = -1;
  if (argc - optind < 1)
    usage();
  if (argc - optind < TPITS)
    fill_pos(p,atoi(argv[optind++]));
  else
    for (i=0;i<TPITS;i++)
      p->a[i] = atoi(argv[optind++]);
  }

int main(int argc, char **argv) {
  char o;
  int verbose = 0;
  int stones = 0;
  int bits = 0;
  int s;
  position p;
  p.s = 0;
  
  while ((o = getopt(argc,argv,"hvt:b:")) != -1)
    switch(o) {
      case 'v': verbose = 1; break;
      case 't': stones = atoi(optarg); break;
      case 'b': bits = atoi(optarg); break;
      default: usage();
      }
  if (argc - optind < 1 || !argv[optind][0] || argv[optind][1])
    usage();
  if (argv[optind][0] != 'e' && (stones || bits))
    usage();

  switch (argv[optind++][0]) {
    case 'm':
      getposition(argc,argv,&p);
      if (argc - optind != 1)
        usage();
      expandmovelist(p,argv[optind],verbose);
      break;
    case 'r':
      getposition(argc,argv,&p);
      if (argc - optind)
        usage();
      trialmoves(p,verbose);
      break;
    case 'e':
      if (argc - optind != 2)
        usage();
      reduce_endgame(stones,bits,argv[optind],argv[optind+1]);
      break;
    case 'd':
      misc();
      break;
    default:
      usage(); 
    }

  return 0;
  }

      

