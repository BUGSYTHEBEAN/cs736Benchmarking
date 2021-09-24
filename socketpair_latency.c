#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#define Ki 1024

#define PARENT_SOCKET 0
#define CHILD_SOCKET 1

#define MESSAGE_SIZE 512 * Ki

void write_until_full(int sockets[], char* message, int socket) {
    ssize_t total_write = 0;
    while (total_write < MESSAGE_SIZE) {
        ssize_t current_write = write(sockets[socket], &message[total_write], MESSAGE_SIZE - total_write);
        if (current_write == -1) {
            perror("Writing to socket failed");
        }
        total_write += current_write;
    }
}

void read_until_full(int sockets[], char* message, int socket) {
    ssize_t total_read = 0;
    while (total_read < MESSAGE_SIZE) {
        ssize_t current_read = read(sockets[socket], &message[total_read], MESSAGE_SIZE - total_read);
        if (current_read == -1) {
            perror("Reading from socket failed");
        }
        total_read += current_read;
    }
}

int main(int argc, char *argv[]) {
    int sockets[2];
    char message[MESSAGE_SIZE];
    struct timespec start;
    struct timespec stop;
    long int min = __INT_MAX__;

    memset(message, 0, sizeof(message));
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) == -1) {
        perror("Creating the socketpair failed");
    }
    int pid = fork();

    for (;;) {
        if (pid != 0) {
            // Parent
            if (clock_gettime(CLOCK_REALTIME, &start) == -1) {
                perror("Starting the clock failed");
            }
            // Send and wait
            write_until_full(sockets, message, PARENT_SOCKET);
            read_until_full(sockets, message, PARENT_SOCKET);
            if (clock_gettime(CLOCK_REALTIME, &stop) == -1) {
                perror("Stopping the clock failed");
            }   
            // Measure time
            long int current = stop.tv_nsec - start.tv_nsec;
            if (current < min && current > 0) {
                min = current;
                printf("Elapsed time in nanoseconds: %lf\n", (float) min / 2.0);
            }
        } else {
            // Child
            read_until_full(sockets, message, CHILD_SOCKET);
            write_until_full(sockets, message, CHILD_SOCKET);
        }
    }
    return (EXIT_SUCCESS);
}
