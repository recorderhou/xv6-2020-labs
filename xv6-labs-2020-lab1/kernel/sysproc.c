#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"

uint64
sys_exit(void)
{
  int n;
  // Fetch the nth 32-bit system call argument.
  // exit(-1) means it ends in an abnormal way
  if(argint(0, &n) < 0)
    return -1;
  // else end the process
  exit(n);
  return 0;  // not reached
}

// get the process's pid
uint64
sys_getpid(void)
{
  return myproc()->pid;
}

// fork
uint64
sys_fork(void)
{
  return fork();
}

// wait
uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

// sbrk
uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

// sleep
uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
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

  if(argint(0, &pid) < 0)
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
	int masks;
	if(argint(0, (int*)&masks) < 0)
    	return -1;
    myproc()->mask = masks;
    return 0;
}

uint64
sys_sysinfo(void)
{
	struct proc *p = myproc();
	struct sysinfo sys_in;
	uint64 st;
	if(argaddr(0, &st) < 0)
		return -1;
		
	sys_in.freemem = get_freemem();
	sys_in.nproc = get_nproc();
	
	if(copyout(p->pagetable, st, (char *)&sys_in, sizeof(sys_in)) < 0)
		return -1;
	return 0;
}
