#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    struct timespec start;
    struct timespec stop;
    long int min = __INT_MAX__;
    
    for (;;) {
        if (timespec_get(&start, TIME_UTC) == -1) {
            perror("Starting the clock failed");
        }
        if (timespec_get(&stop, TIME_UTC) == -1) {
            perror("Stopping the clock failed");
        }
        long int current = stop.tv_nsec - start.tv_nsec;
        if (current < min && current > 0) {
            min = current;
            printf("Elapsed time in nanoseconds: %li\n", min);
        }
    }
    return (EXIT_SUCCESS);
}