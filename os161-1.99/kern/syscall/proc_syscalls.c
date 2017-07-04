#include <types.h>
#include <kern/errno.h>
#include <kern/unistd.h>
#include <kern/wait.h>
#include <kern/fcntl.h>
#include <limits.h>
#include <lib.h>
#include <syscall.h>
#include <current.h>
#include <proc.h>
#include <thread.h>
#include <addrspace.h>
#include <copyinout.h>
#include <synch.h>
#include <vfs.h>
#include <mips/trapframe.h>
#include <machine/trapframe.h>
#include "opt-A2.h"

  /* this implementation of sys__exit does not do anything with the exit code */
  /* this needs to be fixed to get exit() and waitpid() working properly */

void sys__exit(int exitcode) {

  struct addrspace *as;
  struct proc *p = curproc;
  /* for now, just include this to keep the compiler from complaining about
     an unused variable */
  (void)exitcode;

  DEBUG(DB_SYSCALL,"Syscall: _exit(%d)\n",exitcode);

  KASSERT(curproc->p_addrspace != NULL);
  as_deactivate();
  /*
   * clear p_addrspace before calling as_destroy. Otherwise if
   * as_destroy sleeps (which is quite possible) when we
   * come back we'll be calling as_activate on a
   * half-destroyed address space. This tends to be
   * messily fatal.
   */
  as = curproc_setas(NULL);
  as_destroy(as);

  /* detach this thread from its process */
  /* note: curproc cannot be used after this call */
  proc_remthread(curthread);

#if OPT_A2
     lock_acquire(p->lk);
     p->status = _MKWAIT_EXIT(exitcode);
     p->exit = 1;
     cv_broadcast(p->cv,p->lk);
     lock_release(p->lk);
#endif
     
     // if this is the last user process in the system, proc_destroy()
     //   will wake up the kernel menu thread
     proc_destroy(p);
     
     thread_exit();
     // thread_exit() does not return, so we should never get here
     panic("return from thread_exit in sys_exit\n");
    
}


/* stub handler for getpid() system call                */
int
sys_getpid(pid_t *retval)
{
  /* for now, this is just a stub that always returns a PID of 1 */
  /* you need to fix this to make it work properly */
#if OPT_A2
    *retval = curproc->ppid;
#else
    *retval = 1;
#endif
  return(0);
}

/* stub handler for waitpid() system call                */

int
sys_waitpid(pid_t pid,
	    userptr_t status,
	    int options,
	    pid_t *retval)
{
  int exitstatus;
  int result;

  /* this is just a stub implementation that always reports an
     exit status of 0, regardless of the actual exit status of
     the specified process.   
     In fact, this will return 0 even if the specified process
     is still running, and even if it never existed in the first place.

     Fix this!
  */

  if (options != 0) {
    return(EINVAL);
  }
  /* for now, just pretend the exitstatus is 0 */
#if OPT_A2

     struct proc *curp = curproc;
     bool ischild = check(curp, pid);
     if(ischild == false){
     kprintf("waitpid check part");
     return ECHILD;
     }
     
     struct proc * waitp = pick(pid);
     lock_acquire(waitp->lk);
     while(waitp->exit == 0){
     cv_wait(waitp->cv,waitp->lk);
     }
     lock_release(waitp->lk);
     exitstatus = waitp->status;

#else
    /* for now, just pretend the exitstatus is 0 */
    exitstatus = 0;
#endif
    result = copyout((void *)&exitstatus,status,sizeof(int));
    if (result) {
        return(result);
    }
    *retval = pid;
    return(0);
}

#if OPT_A2

 int sys_fork(struct trapframe *tf, pid_t *retval){
 //KASSERT(curproc != NULL); //MARK
 //KASSERT(sizeof(struct trapframe)==(37*4));  //MARK
 
 // step 1
 struct proc* child = proc_create_runprogram("proc");
 if(child == NULL){
 return ENOMEM;
 }
 // step 2
 struct addrspace * caddr = kmalloc(sizeof(struct addrspace));
 if(caddr == NULL){
 proc_destroy(child);
 return ENOMEM;
 }
 child->p_addrspace = caddr;
 
 struct trapframe * ctf = kmalloc(sizeof(struct trapframe));
 if(ctf == NULL){
 kfree(ctf);
 as_destroy(child->p_addrspace);
 proc_destroy(child);
 return ENOMEM;
 }
 
 // step 3
 int result = as_copy(curproc->p_addrspace, &child->p_addrspace);
 if(result){
 kfree(caddr);
 proc_destroy(child);
 }
 memcpy(ctf, tf, sizeof(struct trapframe));
 
 // step 4
 child->parent = curproc;
 unsigned index;
 procarray_setin(&curproc->children, child, &index); // MARK
 //procarray_add(&curproc->children, child, index);
 
 // step 5,6
 result = thread_fork("thread", child, (void *)enter_forked_process, ctf, 0);
 if(result){
 kfree(ctf);
 kfree(caddr);
 kfree(child);
 }
 *retval = child->ppid;
     return 0;
 }

#endif




