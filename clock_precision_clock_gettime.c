#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    struct timespec start;
    struct timespec stop;
    long int min = __INT_MAX__;
    for(;;) {
        clock_gettime(CLOCK_REALTIME, &start);
        clock_gettime(CLOCK_REALTIME, &stop);
        long int current = stop.tv_nsec - start.tv_nsec;
        if (current < min) {
            min = current;
            printf("Elapsed time in nanoseconds: %li\n", min);
        }
    }
    return (EXIT_SUCCESS);
}