#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#define Ki 1024
#define Mi Ki * Ki

#define PARENT_SOCKET 0
#define CHILD_SOCKET 1

#define MESSAGE_COUNT 10000
#define MESSAGE_SIZE ((long int) 512 * Ki)
#define RETURN_MESSAGE_SIZE 1

#define NS_PER_S 1000000000

void write_until_full(int sockets[], char* message, int socket) {
    ssize_t total_write = 0;
    while (total_write < MESSAGE_SIZE) {
        ssize_t current_write = write(sockets[socket], &message[total_write], MESSAGE_SIZE - total_write);
        if (current_write == -1) {
            perror("Writing message failed");
        }
        total_write += current_write;
    }
}

void read_until_full(int sockets[], char* message, int socket) {
    ssize_t total_read = 0;
    while (total_read < MESSAGE_SIZE) {
        ssize_t current_read = read(sockets[socket], &message[total_read], MESSAGE_SIZE - total_read);
        if (current_read == -1) {
            perror("Reading message failed");
        }
        total_read += current_read;
    }
}

int main(int argc, char *argv[]) {
    int sockets[2];
    char message[MESSAGE_SIZE], return_message[RETURN_MESSAGE_SIZE];
    struct timespec start;
    struct timespec stop;
    long int min = __INT_MAX__;

    memset(message, 0, sizeof(message));
    memset(return_message, 0, sizeof(return_message));
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
            for (int i = 0; i < MESSAGE_COUNT; i++) {
                write_until_full(sockets, message, PARENT_SOCKET);
            }
            if (read(sockets[PARENT_SOCKET], return_message, sizeof(return_message)) == -1) {
                perror("Reading return message failed");
            }
            if (clock_gettime(CLOCK_REALTIME, &stop) == -1) {
                perror("Stopping the clock failed");
            }
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
                read_until_full(sockets, message, CHILD_SOCKET);
            }
            if (write(sockets[CHILD_SOCKET], return_message, sizeof(return_message)) == -1) {
                perror("Writing return message failed");
            }
        }
    }
    return (EXIT_SUCCESS);
}