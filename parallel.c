// Kalah toplevel parallism code
// Geoffrey Irving
// 18nov00

#include "parallel.h"

#define BASE_SHIFT 20

typedef struct {
  int tid,jid;   // thief tid and job jid
  } jobstub;

/* global variables */

static int master;      // is this the master process?
static char host[100];  // hostname
static int stid;        // own tid
static int nw;          // number of other workers
static int *wtid;       // tids of other workers

static int nextjid;     // jid of next created job
static int nj;          // number of ready / blocked jobs
static jobinfo **ja;    // job array 

static int nsj;         // number of stolen jobs
static jobstub *sj;     // stolen job ids

static jobinfo *cj;     // current job
stackinfo p_stack[P_STACKDEPTH];  // crunch stack
int p_head, p_tail;               // stack bounds

/* code */

/* job array fiddling */
void add_jobarray(jobinfo *j) {
  ja = realloc(ja,sizeof(jobinfo*) * ++nj);
  ja[nj-1] = j;
  }

void del_jobarray(int j) {
  int i;
  nj--;
  for (i=j;i<nj;i++)
    ja[i] = ja[i+1];
  ja = realloc(ja,sizeof(jobinfo*) * nj);
  }

int find_jobarray(int jid) {
  int i;
  for (i=0;i<nj;i++)
    if (ja[i]->jid == jid)
      return i;
  return -1;
  }

/* stolen job array fiddling */
void add_stolen(int tid, int jid) {
  int i;
  for (i=0;i<nsj;i++)
    if (sj[i].jid == jid) {
      sj[i].tid = tid;
      return;
      }
  sj = realloc(sizeof(jobstub) * ++nsj);
  sj[nsj-1].tid = tid;
  sj[nsj-1].jid = jid;
  }

/* return tid of stolen job given jid */
int find_stolen(int jid) {
  int i;
  for (i=0;i<nsj;i++)
    if (sj[i].jid == jid)
      return sj[i].tid;
  die("Couldn't locate stolen job\n");
  }

/* data packing functions */
void pkjobinfo_active(jobinfo *j) {
  pvm_pkint(&j->jid,6,1);        // jid, tid, pjid, d, a, n
  pvm_pkint(&j->s.k,4,1);        // k,o,r,rd
  pvm_pkbyte(&j->s.p,2+TPITS,1);
  pvm_pkbyte(j->s.m,MAXMVC,1);
  pvm_pkint(j->o,4*PITS,1);      // o, js, ctid, cjid
  }

void upkjobinfo_active(jobinfo *j) {
  pvm_upkint(&j->jid,6,1);       // jid, tid, pjid, d, a, n
  pvm_upkint(&j->s.k,4,1);       // k,o,r,rd
  pvm_upkbyte(&j->s.p,2+TPITS,1);
  pvm_upkbyte(j->s.m,MAXMVC,1);
  pvm_upkint(j->o,4*PITS,1);     // o, js, ctid, cjid
  }

void pkjobinfo_done(jobinfo *j) {
  pvm_pkint(&j->jid,3,1);        // jid, tid, pjid
  pvm_pkint(&j->s.k,4,1);        // k,o,r,rd
  pvm_pkbyte(j->s.m,MAXMVC,1);
  }

void upkjobinfo_done(jobinfo *j) {
  pvm_upkint(&j->jid,3,1);       // jid, tid, pjid
  pvm_upkint(&j->s.k,4,1);       // k,o,r,rd
  pvm_upkbyte(j->s.m,MAXMVC,1);
  }

/* add self to worker list */
void hello() {
  int i,b,n;
  pvmtaskinfo *ta;
  pvm_tasks(0,&n,&ta);
  for (i=0;i<n;i++)
    if (!strcmp(ta[i].ti_a_out,"kalah")) {
      pvm_initsend(0);
      pvm_pkstr(host);
      pvm_send(ta[i].ti_tid,TAG_HELLO);
      break;
      } 
  b = pvm_recv(-1,TAG_WORKERS); 
  if (b < 0)
    die("Failed to receive worker list\n");
  p_handle(b);
  }

/* find local jobs to do */
jobinfo* freejob() {
  jobinfo *j;
  for (i=0;i<nj;i++)
    if (ja[i]->status == JOB_FREE) 
      break;
  if (i < nj) {
    j = ja[i];
    del_jobarray(i);
    }
  else
    j = 0;
  return j;
  }

/* pull new jobs off the top of the stack */
int victim(int tid) {
  int i;
  jobinfo *j; 
  stackinfo *s;

  if (!cj)
    return 0;
  else if (++p_head > p_tail) {
    p_head--;
    return 0;
    }
  else {   // successful steal
    j = cj;
    cj = malloc(sizeof(jobinfo));
    s = p_stack + p_head - 1;
    if (s > p_stack) {   // fill in j if necessary
      j->d = s->d;
      j->a = s->a;
      j->n = s->n;
      j->s = *s->s;
      for (i=0;i<s->n;i++)
        j->o[i] = s->o[i]->o;
      for (i=0;i<s->i;i++)
        j->js[i] = JOB_DONE;
      for (i++;i<s->n;i++)
        j->js[i] = JOB_FREE;
      }

    j->js[s->i] = JOB_TAKEN;
    j->cjid[s->i] = cj->jid = nextjid++;
    j->ctid[s->i] = stid; 
    pvm_initsend(0);
    pkjobinfo_active(j);
    pvm_send(tid,TAG_JOB);
    add_stolen(tid,j->jid);

    cj->tid = tid;
    cj->pjid = j->jid;
    cj->status = JOB_TAKEN;
    free(j);
    return 1;
    }
  }    

/* send a cancel message to a process */
inline void send_cancel(int tid, int jid) {
  pvm_initsend(0);
  pvm_pkint(jid,1,1);
  pvm_send(tid,TAG_CANCEL);
  }

/* cancel all active children of a process */
void cancel_children(jobinfo *j) {
  for (i=0;i<j->n;i++)    // abort child jobs
    if (j->js[i] == JOB_TAKEN)
      send_cancel(j->ctid[i],j->cjid[i]);
  }

/* send off a completed job */
void send_done(jobinfo *j) {
  add_stolen(0,j->jid);
  pvm_initsend(0);
  pkjobinfo_done(j);
  pvm_send(j->tid,TAG_DONE);
  }
 
/* message handling routine */
void p_handle(int b) {
  int i,k,len,tid,tag;
  jobinfo *j;
  jobinfo tj; 
  pvm_bufinfo(b,&len,&tag,&tid);

  switch (tag) {
    case TAG_HELLO:
      if (master) {
        wtid = realloc(wtid,sizeof(int) * ++nw);
        wtid[nw-1] = tid;
        for (i=1;i<nw;i++) {
          pvm_initsend(0);
          k = i << BASE_SHIFT;
          pvm_pkint(&k,1,1);
          pvm_pkint(&nw,1,1);
          pvm_pkint(wtid,nw,1);
          pvm_send(wtid[i],TAG_WORKERS);
          }
        }
      break;

    case TAG_WORKERS: 
      pvm_upkint(&nextjid,1,1);      
      nextjid <<= BASE_SHIFT;
      pvm_upkint(&nw,1,1);
      free(wtid);
      wtid = malloc(sizeof(int) * nw);
      pvm_upkint(wtid,nw,1);
      break;

    case TAG_KILL:
      die("Received TAG_KILL.  Too lazy to do anything useful.\n");

    case TAG_REQUEST:
      if (j = freejob()) {
        pvm_initsend(0);
        pkjobinfo_active(j);
        pvm_send(tid,TAG_JOB);
        add_stolen(tid,j->jid);
        free(j);
        }
      else if (!victim(tid)) {
        pvm_initsend(0);
        pvm_send(tid,TAG_NO_JOB);
        }
      break; 

    case TAG_DONE:
      upkjobinfo_done(&tj);
      if (cj && cj->jid == tj.pjid)
        slow_absorb(cj,&tj.s);
      else if (!tj.jid && master) {
        j = malloc(sizeof(jobinfo)); 
        *j = tj;
        add_jobarray(j);
        }
      else if ((i = find_jobarray(tj.pjid)) >= 0) { 
        slow_absorb(ja[i],&tj.s);
        if (ja[i]->status == JOB_DONE && !(!ja[i]->jid && master)) {
          send_done(ja[i]);
          free(ja[i]); 
          del_jobarray(i);
          }
        }
      else if (tj.tid = find_stolen(tj.pjid))  // if 0, job was cancelled
        send_done(&tj);
      break; 
    
    case TAG_CANCEL:
      pvm_upkint(&k,1,1); 
      if (cj && cj->jid == k) {
        cj->status = JOB_CANCELLED;
        if (p_head > 0) 
          cancel_children(cj);
        p_head = 1000;
        add_stolen(0,cj->jid);
        }
      else if ((i = find_jobarray(k)) >= 0) { 
        cancel_children(ja[i]);
        add_stolen(0,ja[i]->jid); 
        free(ja[i]);
        del_jobarray(i);
        }
      else if (tid = find_stolen(k)) 
        send_cancel(tid,k); 
      break;

    case TAG_STAT:
    default:
      die("Warning: ignoring invalid message\n");
    }
  }

/* steal jobs from other workers */
jobinfo* steal() {
  int i,w,b;
  int len,tid,tag;
  jobinfo *j;
  w = random() % nw;
  for (;;) {
    if (wtid[w] != stid) {
      pvm_initsend(0);
      pvm_send(wtid[w],TAG_REQUEST);
      for (;;) {
        b = pvm_recv(-1,-1);
        pvm_bufinfo(b,&len,&tag,&tid); 
        if (tag == TAG_JOB || tag == TAG_NO_JOB)
          break; 
        else if (tag == TAG_REQUEST) {
          pvm_initsend(0); 
          pvm_send(tid,TAG_NO_JOB);
          }
        else
          p_handle(b);
        }
      if (tag == TAG_JOB)
        break;
      }
    if (++w == nw)
      w = 0;
    }
  
  j = malloc(sizeof(jobinfo));
  upkjobinfo_active(j);
  ha_lookup(ha,&j->s.da,&j->s.p);
  return j;
  }

void slow_absorb(jobinfo *j, c_res *c) {
  int i,r,done;

  if (!j->n) { // toplevel job
    j->s.r = c->r;
    j->s.rd = c->rd;
    strcpy(j->s.m,c->m);
    j->status = JOB_DONE;
    }
  else {
    done = 0;

    for (i=0;i<j->n;i++)
      if (j->o[i] == c->o) {
        j->js[i] = JOB_DONE;
        break;
        }

    r = c->k ? c->r : -c->r;
    if (++c->rd < j->s.rd)
      j->s.rd = c->rd;
    if (j->s.r < r) {
      j->s.r = r;
      *j->s.m = c->o + '0';
      if (c->k)
        strcpy(j->s.m+1,c->m);
      else
        j->s.m[1] = 0;
      if (j->s.r > j->a) {
        STAT(stat.cutoffs++);
        if (j == cj && p_tail > 0) {  // need to cancel a local job
          p_head = 1000;
          j->js[p_stack[0].i] = JOB_DONE;
          }
        cancel_children(j);
        done = 1;
        }
      }

    if (!done) {
      done = 1;
      for (i=1;i<j->n;i++)
        if (j->status != JOB_DONE)
          done = 0;
      }

    if (done) {
      if (j->s.rd > LOWDEPTH && (!j->s.da.t || j->s.rd >= j->s.da.d))
        da_seta(&j->s.da, j->s.rd, *j->s.m-'0', j->s.r, j->s.r > j->a ? DV_LO : DV_HI);
      j->status = JOB_DONE;
      }
    else if (!i && j->status == JOB_BLOCKED)
      j->status = JOB_FREE;
    }
  }

/* parallism aware crunch routine */
void slow_crunch() {
  int i;
  c_res c;

  if (!cj->n) {  // toplevel job 
    if (cj->js[0] == JOB_FREE) {
      i = 0; 
      c.p = cj->s.p;
      c.da = cj->s.da;
      P_PUSH();
      crunch(&c,cj->d,cj->a);
      P_POP();
      cj->s.r = c.r; 
      cj->s.rd = c.rd;
      strcpy(cj->s.m,c.m);
      cj->status = JOB_DONE;
      }
    else
      cj->status = JOB_BLOCKED; 
    }
  else if (cj->js[0] == JOB_TAKEN)
    cj->status = JOB_BLOCKED;
  else {
    for (i=0;i<cj->n;i++)
      if (cj->js[i] == JOB_FREE) {
        c.o = cj->o[i];
        c.p = cj->s.p;
        c.k = move(&c.p,c.o);
        ha_lookup(ha,&c.da,&c.p);
        P_PUSH();
        crunch(&c,cj->d-1,c.k ? cj->a : -cj->a-1);
        P_POP();
        slow_absorb(cj,&c);         
        if (cj->status == JOB_DONE)
          break;
        }
    for (i=0;i<cj->n;i++)
      if (cj->js[i] != JOB_DONE)
        break;
    cj->status = (i < cj->n) ? JOB_BLOCKED : JOB_DONE;
    }
  }
  
/* main parallism function */
void worker() {
  int i,j;

  for(;;) {
    /* find work */
    if (!(cj = freejob()) && !(cj = steal()))
      return;
    if (cj->status == JOB_DONE)   // done
      return;
   
    /* do work */
    cj->status = JOB_TAKEN;
    p_head = p_tail = 0;
    slow_crunch();
   
    /* aftermath */
    switch (cj->status) {
      case JOB_BLOCKED:   
        add_jobarray(cj); 
        cj = 0; 
        break;
    
      case JOB_DONE:      
        if (!cj->jid && master)   // done
          return;
        else {
          send_done(cj);
          free(cj);
          }
        break;
   
      case JOB_CANCELLED: 
        free(cj);
      }
    }
  }    

/* startup function */
void parallel(c_res *s, int d, int a) {
  int i;
  jobinfo *tj = malloc(sizeof(jobinfo));

  /* set up toplevel job */
  tj->jid = 0;
  tj->tid = stid; 
  tj->pjid = 0;
  nextjid = 1;
  tj->d = d;
  tj->a = a;
  tj->n = 0;
  tj->s = *s;
  tj->js[0] = JOB_FREE;
  tj->status = JOB_FREE;
  add_jobarray(tj);

  /* start working */
  worker();

  /* return results */ 
  *s = cj->s;
  free(cj);
  }

/* initialization function */
void init_parallel(int m) {
  /* verify data storage consistency for packing optimization */
  if (sizeof(position) != 2 + TPITS)
    die("sizeof(position) strange\n");
  if (offsetof(c_res,rd) != 3*sizeof(int))
    die("offsets in c_res strange\n");
  if (sizeof(jobinfo) != (6+4*PITS)*sizeof(int) + sizeof(c_res))
    die("sizeof(jobinfo) strange\n");

  srandom(clock());
  nw = nj = nsj = 0;
  wtid = 0;
  ja = 0;
  sj = 0;
  cj = 0;
  master = m;
  gethostname(host,100);
  stid = pvm_mytid();

  if (master) {
    n = 1;
    wtid = malloc(sizeof(int));
    wtid[0] = stid;
    }
  else
    hello();
  }

/* closing function */
void close_parallel() {
  int i;
  pvm_exit();
  free(wtid);
  free(sj);
  for (i=0;i<nj;i++)
    free(ja[i]);
  free(ja);
  }


