#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "ps.h"
#include <stdlib.h>
extern struct proc proc[NPROC];
uint64
sys_exit(void)
{
  int n;
  if (argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0; // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if (argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if (argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if (growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if (argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n)
  {
    if (myproc()->killed)
    {
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if (argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_trace(void)
{
  /* your code goes here */
  int n;
  if (argint(0, &n) < 0)
    return -1;
  myproc()->trace_mask = n;
  return 0;
}

uint64
sys_pinfo(void)
{
  struct psinfo *psinfo = kalloc();
  // struct psinfo *psin;
  if (argaddr(0, (uint64 *)psinfo) < 0)
  {
    return -1;
  }
  struct proc *p;
  int i = 0;
  for (p = proc; p < &proc[NPROC]; p++)
  {
    acquire(&p->lock);
    psinfo->active[i] = 1;
    psinfo->pid[i] = p->pid;
    psinfo->states[i] = p->state;
    strncpy(psinfo->name[i], p->name, 16);
    int num_used = countmapped(p->pagetable);
    psinfo->num_used_pages[i] = num_used;
    i++;
    release(&p->lock);
  }
  // if(copyout())
  return 0;
}