#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "threadtools.h"

int timeslice;
sigset_t base_mask, tstp_mask, alrm_mask;
struct tcb *ready_queue[THREAD_MAX], *waiting_queue[THREAD_MAX];
int rq_size, rq_current, wq_size;
jmp_buf sched_buf;
struct thread_status thread_status_array[THREAD_MAX] = {0};

/* prototype of the thread functions */
void fibonacci(int, int);
void stir(int, int);
void worker(int, int);
void coordinator(int, int);

/*
 * This function turns stdin, stdout, and stderr into unbuffered I/O, so:
 *   - you see everything your program prints in case it crashes
 *   - the program behaves the same if its stdout doesn't connect to a terminal
 */
void unbuffered_io() {
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);
}

/*
 * Initializes the signal masks and the signal handler.
 */
void init_signal() {
    /* initialize the signal masks */
    sigemptyset(&base_mask);
    sigaddset(&base_mask, SIGTSTP);
    sigaddset(&base_mask, SIGALRM);
    sigemptyset(&tstp_mask);
    sigaddset(&tstp_mask, SIGTSTP);
    sigemptyset(&alrm_mask);
    sigaddset(&alrm_mask, SIGALRM);

    /* initialize the signal handlers */
    signal(SIGTSTP, sighandler);
    signal(SIGALRM, sighandler);

    /* block both SIGTSTP and SIGALRM */
    sigprocmask(SIG_SETMASK, &base_mask, NULL);
}

/*
 * Threads are created nowhere else but here.
 */
void init_threads(int fib_arg, int stir_type, int stir_n, int sum_n, int sum_k) {
	int id = 0;
    if (fib_arg >= 0) {
		thread_create(fibonacci, id++, fib_arg);
	}
    if ((stir_type == 1 || stir_type == 2) && stir_n >= 0) {
        thread_create(stir, id++, (stir_type << 16) | stir_n);
	}
	if (sum_n > 0 && sum_k > 0) {
		thread_create(coordinator, id++, (sum_n << 16) | sum_k);
		int left = 1, right, sum_range = sum_n / sum_k;
		for (int i = 1; i <= sum_k; i++) {
			if (i < sum_k) {
				right = left + sum_range - 1;
			} else {
				right = sum_n;
			}
			thread_create(worker, id++, (left << 16) | right);
			left += sum_range;
		}
	}
}

/*
 * Calls the scheduler to begin threading.
 */
void start_threading() {
    alarm(timeslice);
    scheduler();
}

int main(int argc, char *argv[]) {
    if (argc != 7) {
        printf("Usage: %s [timeslice] [fib_arg] [stir_type] [stir_n] [sum_n] [sum_k]\n", argv[0]);
        exit(1);
    }
	timeslice = atoi(argv[1]);
	int fib_arg = atoi(argv[2]);
	int stir_type = atoi(argv[3]);
	int stir_n = atoi(argv[4]);
	int sum_n = atoi(argv[5]);
	int sum_k = atoi(argv[6]);
    unbuffered_io();
    init_signal();
	init_threads(fib_arg, stir_type, stir_n, sum_n, sum_k);
	start_threading();
}
