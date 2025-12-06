// scheduler.c
#include "threadtools.h"
#include <sys/signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

/* Signal handler: print caught signal, reset alarm, then jump to scheduler. */
void sighandler(int signo) {
    sigprocmask(SIG_SETMASK, &base_mask, NULL);

    if (signo == SIGTSTP) {
        printf("caught SIGTSTP\n");
    } else if (signo == SIGALRM) {
        printf("caught SIGALRM\n");
        alarm(timeslice);
    }
    siglongjmp(sched_buf, 1);
}

void scheduler() {
    int reason = sigsetjmp(sched_buf, 1);

    if (reason == 0) {
        rq_current = 0;
        siglongjmp(RUNNING->environment, 1);
    } else if (reason == 1) {
        rq_current = (rq_current + 1) % rq_size;
    } else if (reason == 2) {
        // join：移除當前執行緒
        int idx = rq_current;
        if (idx < rq_size - 1)
            ready_queue[idx] = ready_queue[rq_size - 1];
        rq_size--;
        if (rq_current == rq_size)
            rq_current = 0;
    } else if (reason == 3) {
        // exit：移除執行緒、喚醒等待它的 
        int finished_id = RUNNING->id;
        free(ready_queue[rq_current]);
        if (rq_current < rq_size - 1)
            ready_queue[rq_current] = ready_queue[rq_size - 1];
        rq_size--;
        if (rq_current == rq_size)
            rq_current = 0;

        // 喚醒等待該 thread 的其他人
        for (int i = 0; i < wq_size; i++) {
            if (waiting_queue[i]->waiting_for == finished_id) {
                waiting_queue[i]->waiting_for = -1;
                ready_queue[rq_size++] = waiting_queue[i];
                for (int j = i; j < wq_size - 1; j++)
                    waiting_queue[j] = waiting_queue[j + 1];
                wq_size--;
                i--;
            }
        }
    }

    if (rq_size == 0) return;
    siglongjmp(RUNNING->environment, 1);
}