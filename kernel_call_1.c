#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    struct timespec start;
    struct timespec stop;
    long int min = __INT_MAX__;
    for(;;) {
        timespec_get(&start, TIME_UTC);
        getpid();
        timespec_get(&stop, TIME_UTC);
        long int current = stop.tv_nsec - start.tv_nsec;
        if (current < min && current > 0) {
            min = current;
            printf("Elapsed time in nanoseconds: %li\n", min);
        }
    }
    return (EXIT_SUCCESS);
}