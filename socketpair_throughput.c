#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#define Ki 1024
#define Mi Ki*Ki

#define PARENT_SOCKET 0
#define CHILD_SOCKET 1

#define MESSAGE_COUNT 10000
#define MESSAGE_SIZE ((long int) 512*Ki)
#define RETURN_MESSAGE_SIZE 1

#define NS_PER_S 1000000000

int main(int argc, char *argv[]) {
    int socket[2];
    char message[MESSAGE_SIZE], return_message[RETURN_MESSAGE_SIZE];
    struct timespec start;
    struct timespec stop;
    long int min = __INT_MAX__;

    memset(message, 0, sizeof(message));
    memset(return_message, 0, sizeof(return_message));
    socketpair(AF_UNIX, SOCK_STREAM, 0, socket);
    int pid = fork();
    
    for(;;) {
        if (pid != 0) {
            // Parent
            clock_gettime(CLOCK_REALTIME, &start);
            // Send and wait
            for (int i = 0; i < MESSAGE_COUNT; i++) {
                write(socket[PARENT_SOCKET], message, sizeof(message));
            }
            read(socket[PARENT_SOCKET], return_message, sizeof(return_message));
            clock_gettime(CLOCK_REALTIME, &stop);
            // Measure time
            long int current = stop.tv_nsec - start.tv_nsec;
            if (current < min && current > 0) {
                min = current;
                long double MiB = ((long double) (MESSAGE_COUNT * MESSAGE_SIZE)) / ((long double) Mi);
                long double s = ((long double) min) / ((long double) NS_PER_S);
                printf("Throughput in MiB/s: %Lf\n", MiB / s);
            }
        } else {
            // Child
            for (int i = 0; i < MESSAGE_COUNT; i++) {
                read(socket[CHILD_SOCKET], message, sizeof(message));
            }
            write(socket[CHILD_SOCKET], return_message, sizeof(return_message));
        }
    }
    return (EXIT_SUCCESS);
}