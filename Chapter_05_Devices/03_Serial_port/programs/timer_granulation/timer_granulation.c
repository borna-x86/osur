#include <stdio.h>
#include "time.h"
#include <kernel/kprint.h>

#include <types/time.h>


int timer_granulation() {
    printf("Demostrates timers & alarms with 200us granulation\n");
    printf("For some reason, maybe because of frequent interrupts, ");
    printf("the qemu simulation runs slower, so the e.g. 3 second alarm takes 5 seconds,");
    printf("however, the system time measured is correct\n");

    printf("Will print current time, the nanosecond part that should always be a multiple of 200 000, since that is the granulation.\n");
    printf("Press 'j' key to start!, will stop after 500 iterations, then the timer simulation will start!\n\n");
    while(getchar() != 'j');

    timespec_t clock;
    int iters;
    for(iters = 0; iters < 500; ++iters) {
        clock_gettime ( CLOCK_REALTIME, &clock );
        printf("ns: %d\n", clock.tv_nsec);
    }

    return 0;
}