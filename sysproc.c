#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

#include "syscall.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
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

int
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

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}


static char * syscalls_names[] = {
[SYS_fork]         "sys_fork",
[SYS_exit]         "sys_exit",
[SYS_wait]         "sys_wait",
[SYS_pipe]         "sys_pipe",
[SYS_read]         "sys_read",
[SYS_kill]         "sys_kill",
[SYS_exec]         "sys_exec",
[SYS_fstat]        "sys_fstat",
[SYS_chdir]        "sys_chdir",
[SYS_dup]          "sys_dup",
[SYS_getpid]       "sys_getpid",
[SYS_sbrk]         "sys_sbrk",
[SYS_sleep]        "sys_sleep",
[SYS_uptime]       "sys_uptime",
[SYS_open]         "sys_open",
[SYS_write]        "sys_write",
[SYS_mknod]        "sys_mknod",
[SYS_unlink]       "sys_unlink",
[SYS_link]         "sys_link",
[SYS_mkdir]        "sys_mkdir",
[SYS_close]        "sys_close",

[SYS_print_count]  "sys_print_count",
[SYS_toggle]       "sys_toggle",
[SYS_add]          "sys_add",
[SYS_ps]           "sys_ps",
};

int syscalls_count[N_SYSCALLS + 1];
int sys_print_count() {
    for (int i = 0; i < N_SYSCALLS + 1; ++i) {
        if (syscalls_count[i]) {
            cprintf("%s %d\n", syscalls_names[i], syscalls_count[i]);
        }
    }
    return 0;
}

int trace_state;
int sys_toggle() {
    trace_state ^= 1;
    for (int i = 0; i < N_SYSCALLS; ++i) {
        syscalls_count[i] = 0;
    }
    return 0;
}


int sys_add() {
    int a;
    if (argint(0, &a) < 0) return -1;

    int b;
    if (argint(1, &b) < 0) return -1;

    return a + b;
}

int sys_ps() {
    return ps();
}
