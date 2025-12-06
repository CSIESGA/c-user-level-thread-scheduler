// threads.c
#include "threadtools.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void fibonacci(int id, int arg) {
    thread_setup(id, arg);
    
    for (RUNNING->i = 1; RUNNING->i <= RUNNING->arg; RUNNING->i++) {
        if (RUNNING->i <= 2) {
            RUNNING->x = RUNNING->y = 1;
        } else {
            int tmp = RUNNING->y;
            RUNNING->y = RUNNING->x + RUNNING->y;
            RUNNING->x = tmp;
        }
        printf("%d %d\n", RUNNING->id, RUNNING->y);
        sleep(1);

        if (RUNNING->i == RUNNING->arg) {
            thread_exit(0);
        } else {
            thread_yield();
        }
    }
}

void stir(int id, int arg) {
    thread_setup(id, arg);
    // x is stir type
    // y is stir_n
    /* Initialize row 0: s(0,0)=1 */
    RUNNING->num[0] = 1;
    for (int k = 1; k < 11; k++) {
        RUNNING->num[k] = 0;
    }

    /* Iteratively compute Stirling rows i = 1..n */
    for (RUNNING->i = 0; RUNNING->i <= RUNNING->y; RUNNING->i++) {
        RUNNING->num[0] = (RUNNING->i == 0)? 1 : 0;  // s(i,0)=0 for i>0
        RUNNING->num[RUNNING->i] = 1;
        if (RUNNING->x == 1) {
            /* Stirling numbers of the first kind */
            for (int k = RUNNING->i - 1; k >= 1; k--) {
                int newVal = (RUNNING->i - 1) * RUNNING->num[k] + RUNNING->num[k - 1];
                RUNNING->num[k] = newVal;
            }
        } else {
            /* Stirling numbers of the second kind */
            for (int k = RUNNING->i - 1; k >= 1; k--) {
                int newVal = k * RUNNING->num[k] + RUNNING->num[k - 1];
                RUNNING->num[k] = newVal;
            }
        }

        /* Print the row for i */
        printf("%d", RUNNING->id);
        for (int j = 0; j <= RUNNING->i; j++) {
            printf(" %d", RUNNING->num[j]);
        }
        printf("\n");
        sleep(1);

        if (RUNNING->i == RUNNING->y) {
            thread_exit(0);
        } else {
            thread_yield();
        }
    }
}

void worker(int id, int arg) {
    thread_setup(id, arg);
    // x is left
    // y is right
    // z is total summation
    for (RUNNING->i = RUNNING->x; RUNNING->i <= RUNNING->y; RUNNING->i++) {
        RUNNING->z += RUNNING->i;
        printf("%d %d\n", RUNNING->id, RUNNING->z);
        sleep(1);
        if (RUNNING->i == RUNNING->y) {
            thread_exit(RUNNING->z);
        } else {
            thread_yield();
        }
    }
}

void coordinator(int id, int arg) {
    thread_setup(id, arg);
    // x is sum_n but due to sum_n is not used here, so I changed x to store retval
    // y is sum_k
    // z is total summation
    for (RUNNING->i = 1; RUNNING->i <= RUNNING->y; RUNNING->i++) {
        thread_join(RUNNING->id + RUNNING->i, RUNNING->x);
        RUNNING->z += RUNNING->x;
        printf("%d %d\n", RUNNING->id, RUNNING->z);
        sleep(1);
        if (RUNNING->i == RUNNING->y) {
            thread_exit(RUNNING->z);
        } else {
            thread_yield();
        }
    }
}