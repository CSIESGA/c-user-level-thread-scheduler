#ifndef THREADTOOL
#define THREADTOOL
#include <setjmp.h>
#include <sys/signal.h>

#define THREAD_MAX 16 // maximum number of threads created

struct tcb {
    int id;                  // the thread id
    jmp_buf environment;     // where the scheduler should jump to
    int arg;                 // argument to the function
    int i, x, y, z, num[11]; // declare the variables you wish to keep between switches
    int waiting_for;         // id of the thread this thread is waiting for (-1 if not waiting)
};

struct thread_status {
    int exists;              // whether this thread exists (0 = not exists, 1 = exists)
    int has_exited;          // whether this thread has exited (0 = not exited, 1 = exited)
    int return_value;        // return value of the thread
};

extern int timeslice;
extern jmp_buf sched_buf;
extern struct tcb *ready_queue[THREAD_MAX], *waiting_queue[THREAD_MAX];
/*
 * rq_size: size of the ready queue
 * rq_current: current thread in the ready queue
 * wq_size: size of the waiting queue
 */
extern int rq_size, rq_current, wq_size;
/*
 * thread_status_array: stores status information for each thread, indexed by thread id
 */
extern struct thread_status thread_status_array[THREAD_MAX];
/*
 * base_mask: blocks both SIGTSTP and SIGALRM
 * tstp_mask: blocks only SIGTSTP
 * alrm_mask: blocks only SIGALRM
 */
extern sigset_t base_mask, tstp_mask, alrm_mask;
/*
 * Use this to access the running thread.
 */
#define RUNNING (ready_queue[rq_current])

void sighandler(int signo);
void scheduler();

// TODO:
#define thread_create(func, id, arg) { \
}

#define thread_setup(id, arg) { \
}

#define thread_exit() { \
}

#define thread_yield() { \
}

#define thread_join(id, retval) { \
}

#endif // THREADTOOL
