// Miscellaneous useful kalah operations 
// Geoffrey Irving
// 18aug00

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>

#include "rules.h"

void usage() {
  fprintf(stderr,"Usage: twiddle [OPTIONS] m n s\n");
  fprintf(stderr,"       twiddle [OPTIONS] r n\n");
  fprintf(stderr,"       twiddle [OPTIONS] d\n");
  fprintf(stderr,"Options:\n");
  fprintf(stderr,"  -h     display this help screen and quit\n");
  fprintf(stderr,"  -v     verbose output\n");
  fprintf(stderr,"Commands:\n");
  fprintf(stderr,"  m n s  expand movelist s starting with n stones\n");
  fprintf(stderr,"  r n    make trial moves starting with n stones\n");
  fprintf(stderr,"  d      display miscellaneous information\n");
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

int main(int argc, char** argv) {
  char o;
  int verbose = 0;
  int s;
  position p;
  p.s = 0;
  
  while ((o = getopt(argc,argv,"hv")) != -1)
    switch(o) {
      case 'v': verbose = 1; break;
      default: usage();
      }
  if (argc - optind < 1 || !argv[optind][0] || argv[optind][1])
    usage();

  switch (argv[optind++][0]) {
    case 'm':
      if (argc - optind != 2)
        usage();
      fill_pos(&p,atoi(argv[optind++]));
      expandmovelist(p,argv[optind],verbose);
      break;
    case 'r':
      if (argc - optind != 1)
        usage();
      fill_pos(&p,atoi(argv[optind++]));
      trialmoves(p,verbose);
      break;
    case 'd':
      misc();
      break;
    default:
      usage(); 
    }

  return 0;
  }

      

