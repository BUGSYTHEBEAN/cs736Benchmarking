#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#define K 1024
#define PARENT_SOCKET 0
#define CHILD_SOCKET 1
#define MESSAGE_SIZE 512*K

int main(int argc, char *argv[]) {
    int socket[2];
    char message[MESSAGE_SIZE];
    struct timespec start;
    struct timespec stop;
    long int min = __INT_MAX__;
    memset(message, 0, sizeof(message));
    socketpair(AF_UNIX, SOCK_STREAM, 0, socket);
    int pid = fork();
    for(;;) {
        if (pid != 0) {
            // Parent
            clock_gettime(CLOCK_REALTIME, &start);
            // Send and wait
            write(socket[PARENT_SOCKET], message, sizeof(message));
            read(socket[PARENT_SOCKET], message, sizeof(message));
            clock_gettime(CLOCK_REALTIME, &stop);
            // Measure time
            long int current = stop.tv_nsec - start.tv_nsec;
            if (current < min && current > 0) {
                min = current;
                printf("Elapsed time in nanoseconds: %lf\n", (float) min / 2.0);
            }
        } else {
            // Child
            read(socket[CHILD_SOCKET], message, sizeof(message));
            write(socket[CHILD_SOCKET], message, sizeof(message));
        }
    }
    return (EXIT_SUCCESS);
}