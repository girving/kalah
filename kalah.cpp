// Kalah main code 
// Geoffrey Irving
// 2sep0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "params.h"
#include "rules.h"
#include "crunch.h"

void usage() {
  fprintf(stderr,"Usage: kalah [OPTIONS] s n\n");
  fprintf(stderr,"       kalah [OPTIONS] p p0 ... p%d\n",LPIT);
  fprintf(stderr,"Starting commands:\n");
  fprintf(stderr,"  s n    start with n stones in each pit\n");
  fprintf(stderr,"  p ...  start with explicit position\n");
  fprintf(stderr,"Options:\n");
  fprintf(stderr,"  -d n   search depth (default 200)\n");
  fprintf(stderr,"  -r n   guess for minimax value\n");
  fprintf(stderr,"  -j     skip iterative deepening\n");
  fprintf(stderr,"  -l     single test call at full depth\n");
  fprintf(stderr,"  -g     play a complete game\n");
  fprintf(stderr,"  -v     verbose output\n");
  fprintf(stderr,"  -V     extremely verbose output\n");
  fprintf(stderr,"  -t n   transposition table size (required)\n");
  fprintf(stderr,"  -e n   endgame database size (required)\n");
  fprintf(stderr,"  -T f   tranposition table file (defaults to none)\n");
  fprintf(stderr,"  -E f   endgame database file (defaults to endgame.dat)\n");
  fprintf(stderr,"Examples:\n");
  fprintf(stderr,"  kalah -vt 20 -e 18 s 3\n");
  exit(0);
  }

int main(int argc, char **argv) {
  char o;
  int guess, game = 0, verbose = 0;
  int flags = 0, hashsize = 0, endgamesize = 0;
  char *hashfile = 0;
  char *endgamefile = "endgame.dat";

  position p;
  int i,s,r,rd;
  int d = 200;
  char *m;
  char moves[400];

  /* parse options */
  while ((o = getopt(argc,argv,"hd:r:jlgvVt:e:T:E:")) != -1)
    switch (o) {
      case 'd': d = atoi(optarg); break;
      case 'r': guess = atoi(optarg); break;
      case 'j': flags |= SF_JUMP; break;
      case 'l': flags |= SF_SINGLE; break;
      case 'g': game = 1; break;
      case 'v': flags |= SF_VERBOSE; verbose = 1; break;
      case 'V': flags |= SF_FLOOD; verbose = 2; break;
      case 't': hashsize = atoi(optarg); break;
      case 'e': endgamesize = atoi(optarg); break;
      case 'T': hashfile = optarg; break;
      case 'E': endgamefile = optarg; break;
      default: usage();
      }
  if (!hashsize || !endgamesize || argc - optind < 1 || strlen(argv[optind]) != 1)
    usage();

  /* parse initial position arguments */
  switch (argv[optind++][0]) {
    case 's':
      if (argc - optind != 1) usage();
      p = position(atoi(argv[optind]));
      break;
    case 'p':
      if (argc - optind != TPITS) usage();
      for (i=0;i<TPITS;i++)
        p.a[i] = atoi(argv[optind++]);
      break;
    default:
      usage();
    }

  init_hash(hashsize,hashfile);
  init_endgame(endgamesize,endgamefile);
  init_stat();

  s = 0;
  r = guess;
  m = moves;
  printq(stdout,0,p);
  while (p.w < 0) {
    r = solve(s,m,&p,d,&rd,r,flags);
    printf("Move: s %d, m %s, r %d, rd %d\n",s,m,r,rd);
    if (!game) break;
    do { 
      if (!p.move(s,*m++-'0')) {
        s = 1 - s;
        r = -r;
        memmove(m+1,m,strlen(m)+1);
        *m++ = '-';
        }
      if (game)
        printq(stdout,s,p);
      } while (*m && d == 200);
    }

  if (game) {
    printf("Winner: s %d, r %d\n",p.w,p.rate(p.w));
    *m-- = 0;
    if (*m == '-')
      *m = 0;
    printf("Moves: %s\n",moves);
    }

  print_stat(stdout,"");
  save_hash(hashfile);
  return 0;
  }


