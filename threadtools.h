// threadtools.h
#ifndef THREADTOOL
#define THREADTOOL
#include <setjmp.h>
#include <sys/signal.h>
#include <stdio.h>
#include <stdlib.h>

#define THREAD_MAX 16

struct tcb {
    int id;
    jmp_buf environment;
    int arg;
    int i, x, y, z, num[11];
    int waiting_for;
};

struct thread_status {
    int exists;
    int has_exited;
    int return_value;
};

extern int timeslice;
extern jmp_buf sched_buf;
extern struct tcb *ready_queue[THREAD_MAX], *waiting_queue[THREAD_MAX];
extern int rq_size, rq_current, wq_size;
extern struct thread_status thread_status_array[THREAD_MAX];
extern sigset_t base_mask, tstp_mask, alrm_mask;
void sighandler(int signo);
void scheduler();

#define RUNNING (ready_queue[rq_current])

// thread_create: simply call the function (which will call thread_setup immediately).
#define thread_create(func, id, arg) {        \
    func(id, arg);                            \
}

// thread_setup: initialize TCB, enqueue to ready, print ID and function name.
#define thread_setup(id, arg) {                               \
    struct tcb *t = malloc(sizeof(struct tcb));              \
    t->id = (id); t->arg = (arg);                             \
    t->waiting_for = -1;                                      \
    t->x = arg >> 16;                                         \
    t->y = arg & 0xFFFF;                                      \
    t->z = 0;                                                 \
    thread_status_array[id].exists = 1;                       \
    thread_status_array[id].has_exited = 0;                   \
    ready_queue[rq_size++] = t;                               \
    printf("%d %s\n", id, __func__); /* print "[id] funcname" */ \
    if (!sigsetjmp(t->environment, 1)) return;                \
}

// thread_exit: mark as exited and jump to scheduler (reason 3).
#define thread_exit(val) {                                          \
    thread_status_array[RUNNING->id].has_exited = 1;               \
    thread_status_array[RUNNING->id].return_value = (val);         \
    siglongjmp(sched_buf, 3); /* no return */                      \
}

// thread_yield: save context, unblock signals, then reblock if no pending.
#define thread_yield() {                                         \
    if (!sigsetjmp(RUNNING->environment, 1)) {                   \
        /* Unblock SIGTSTP and SIGALRM so handler can catch them. */ \
        sigprocmask(SIG_UNBLOCK, &tstp_mask, NULL);              \
        sigprocmask(SIG_UNBLOCK, &alrm_mask, NULL);              \
        /* Re-block both signals (base_mask blocks both) */      \
        sigprocmask(SIG_SETMASK, &base_mask, NULL);              \
    }                                                            \
}

// thread_join: if target exited, get retval; otherwise wait and jump to scheduler.
#define thread_join(id, retval) {                                        \
    if (thread_status_array[id].has_exited) {                            \
        retval = thread_status_array[id].return_value;                \
    } else {                                                             \
        RUNNING->waiting_for = (id);                                     \
        waiting_queue[wq_size++] = RUNNING;                              \
        if (!sigsetjmp(RUNNING->environment, 1)) {                       \
            siglongjmp(sched_buf, 2); /* scheduler: reason 2 (join) */  \
        }                                                                \
        retval = thread_status_array[id].return_value;                \
    }                                                                    \
}

#endif // THREADTOOL