// Kalah lowlevel hash code
// Geoffrey Irving
// 4sep0

// Hash function written by Bob Jenkins

#ifndef __MIX_H 
#define __MIX_H

#include <limits.h>
#include "params.h"
#include "rules.h"

#define B32 0xffffffffL
#if ULONG_MAX == B32 
typedef unsigned long ub4;
#elif UINT_MAX == B32
typedef unsigned int ub4;
#elif USHRT_MAX == B32
typedef unsigned short ub4;
#endif

#if ULONG_MAX == B32
typedef unsigned long long ub8;
#define set_ub8(d,a,b) *(ub4*)&d = a; *((ub4*)&d+1) = b;
#else
typedef unsigned long ub8; 
#define set_ub8(d,a,b) (d = a | ((ub8)b << 32))
#endif

#endif



#ifdef __hash_lowlevel

#define HASHOVERFLOW (1L<<30)
#define HASHOVERFLOW64 (((ub8)1<<30) & ((ub8)1<<62))

static inline long packpits(char *a) {
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

  return a[0] | (long)a[1]<<5 
  #if PITS > 2
              | (long)a[2]<<10 
  #if PITS > 3
              | (long)a[3]<<15
  #if PITS > 4
              | (long)a[4]<<20 
  #if PITS > 5
              | (long)a[5]<<25 
  #endif
  #endif
  #endif
  #endif
  ;
  }

static inline ub4 mix32(register ub4 a, register ub4 b) {
  register ub4 k = 0xfca09587;                
  a += 0x9e3779b9;
  b += 0x9e3779b9;
  a -= b; a -= k; a ^= (k>>13);  
  b -= k; b -= a; b ^= (a<<8);   
  k -= a; k -= b; k ^= (b>>13);  
  a -= b; a -= k; a ^= (k>>12);  
  b -= k; b -= a; b ^= (a<<16);  
  k -= a; k -= b; k ^= (b>>5);   
  a -= b; a -= k; a ^= (k>>3);   
  b -= k; b -= a; b ^= (a<<10);  
  k -= a; k -= b; k ^= (b>>15);
  return k;
  }

static inline ub8 mix64(register ub8 k) {
  k += ~(k << 34);
  k ^=  (k >> 29);
  k += ~(k << 11);
  k ^=  (k >> 14);
  k += ~(k <<  7);
  k ^=  (k >> 28);
  k += ~(k << 26);
  return k;
  }

#if HASH == ZOBRIST

ub4 zobtable[][32];

static inline ub4 zobrist(position p) {
  int i;
  register ub4 k = 0;
  for (i=0;i<PITS;i++)
    k ^= zobtable[i][bin(&p,i)]; 
  for (i=0;i<PITS;i++)
    k ^= zobtable[i+PITS][o_bin(&p,i)]; 
  return k;
  } 

ub4 zobtable[2*PITS][32] = {
  0xe7bdc877, 0xf876d508, 0x3b6a9981, 0x04a35bea,
  0x71b445c0, 0x6eb7be49, 0x54b3250f, 0xf34da304,
  0x19f664e2, 0xf5f6a192, 0x497f301c, 0x19b99b7e,
  0x8868d7a5, 0x4dd25fd9, 0x5f3b2305, 0xc26887e0,
  0x0c783a48, 0x6925150b, 0xb71eb98c, 0xa44c6902,
  0x6132680e, 0x1fd0849b, 0x1610d8ca, 0xbf163a83,
  0xc36a9bfb, 0x859a8aa2, 0x4501e638, 0xd83cd47b,
  0xf2673286, 0x2eb55929, 0x2790d371, 0x6312c7fd,
  
  0x1cdfcb12, 0xa58bd498, 0x7f26d066, 0x853accf2,
  0x1244fffa, 0x6f795ec6, 0xcc644894, 0xe1b73734,
  0x905a46e1, 0xf7f21789, 0x11b1addd, 0x0cc2fd8c,
  0xb63edf62, 0x2b2f396d, 0x26706d14, 0x545c5c04,
  0xb4c6cbdd, 0x9e5ce1bc, 0xf8bae068, 0x7bdecb10,
  0x68c93afc, 0x68d1fefb, 0xbcb5854f, 0x0f83726d,
  0x0d7236d0, 0xe5a4676d, 0xd95c53d4, 0x201a05f7,
  0x90249b86, 0x375c3877, 0xa57ecaf2, 0x3c282ea4,
#if PITS > 2
  0x40a45dfd, 0x0636f29a, 0x9f999935, 0xe5e2dfa6,
  0xc4952a4f, 0x7e169406, 0xabf40336, 0x271cf690,
  0xa3559020, 0x094b9334, 0xfb29cfbf, 0x880716aa,
  0x87b8876b, 0xbcc20813, 0x7c44ec68, 0x1fcfad17,
  0x77114611, 0x1a11eb22, 0x646eb710, 0xdf582571,
  0x1612f627, 0xf66c157a, 0xfbc73761, 0xaa62f7e8,
  0xfd46a3c5, 0xd0ba0098, 0x03dd5f81, 0x98e69123,
  0xd4b1266f, 0xb0c0c229, 0xbaa362f0, 0xd4ceac9a,
#if PITS > 3
  0x82f312ab, 0x28622674, 0xfeadc7a2, 0xdb0432b6,
  0xea863b90, 0x3649700f, 0xecce524f, 0x00a72845,
  0x1a6a9994, 0xbd4bf502, 0xff4234c6, 0x44a8ffdb,
  0xa1c0b228, 0x10215f10, 0x0a9467ec, 0xdb800f6d,
  0xe23834f7, 0x83898059, 0x8337002d, 0x1b8d709c,
  0x83a48e73, 0x77455e35, 0x163c9a15, 0x11d8102b,
  0x6979326b, 0xc0b13fc0, 0x56548b3a, 0x48df45b9,
  0x7123659e, 0xc71858f5, 0xdf761b52, 0xe2343661,
#if PITS > 4
  0x37e17497, 0x9fd2ddbd, 0xcdedfb0c, 0x7dd49808,
  0x603effa8, 0xfccb3b8a, 0xa5c2ced5, 0xb9329ae3,
  0xe25e5f79, 0xaac4e075, 0xb53e0c8b, 0x10f3e2a3,
  0x516bfd67, 0x57ff918a, 0x3b50987d, 0x3104d500,
  0x2fdc6b71, 0x96ff00d4, 0x21e66215, 0x7bf1e095,
  0x976c3bac, 0xec6dc96f, 0xe0175d30, 0xd2f21f64,
  0x215b8298, 0x76eb693b, 0xa655a72d, 0x2fb0df0e,
  0x68f94bb0, 0x01480124, 0xfe74916c, 0x84f4e644,
#if PITS > 5
  0x1e5c6505, 0x7885d294, 0xa609d057, 0x6d879441,
  0x63b777ae, 0x24d429f7, 0x7ae1cf90, 0x51b6d20b,
  0xe04f9173, 0x85b3ef99, 0xb41073a9, 0xb067dc4a,
  0x98faf94d, 0xd92484d2, 0x14c1999d, 0xb79584e5,
  0x50d48217, 0xa6edb182, 0xb110ebf6, 0x86551925,
  0x5b4a0606, 0xad7f31d3, 0xe2d9dee9, 0xdbde9491,
  0x7d55174c, 0x1ea2265d, 0x47a569d3, 0xaa3306ae,
  0xc8977031, 0x3daa721f, 0xb208c34d, 0xb917f45c,
#endif
#endif
#endif
#endif
  };

#endif

#endif
