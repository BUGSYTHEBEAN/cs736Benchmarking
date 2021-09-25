#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#define Ki 1024

#define MESSAGE_SIZE 512 * Ki
#define PORT 6969

#define IP "128.105.37.156"

void read_until_full(int connection, char* message) {
    ssize_t total_read = 0;
    while (total_read < MESSAGE_SIZE) {
        ssize_t current_read = read(connection, &message[total_read], MESSAGE_SIZE - total_read);
        if (current_read == -1) {
            perror("Reading from TCP failed");
        }
        total_read += current_read;
    }
}

void write_until_full(int connection, char* message) {
    ssize_t total_write = 0;
    while (total_write < MESSAGE_SIZE) {
        ssize_t current_write = write(connection, &message[total_write], MESSAGE_SIZE - total_write);
        if (current_write == -1) {
            perror("Writing to TCP failed");
        }
        total_write += current_write;
    }
}

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in server, client;
    char message[MESSAGE_SIZE];
    struct timespec start;
    struct timespec stop;
    long int min = __INT_MAX__;

    memset(message, 0, sizeof(message));
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Creating the TCP socket failed");
    }

    server.sin_addr.s_addr = inet_addr(IP);
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);    

    if (connect(sock, (struct sockaddr*) &server, sizeof(server)) == -1) {
        perror("Connection to server failed");
    }

    for (;;) {
        // Start clock
        if (clock_gettime(CLOCK_REALTIME, &start) == -1) {
            perror("Starting the clock failed");
        }
        // Write
        write_until_full(sock, message);
        // Read
        read_until_full(sock, message);
        // End clock
        if (clock_gettime(CLOCK_REALTIME, &stop) == -1) {
            perror("Stopping the clock failed");
        }   
        // Measure time
        long int current = stop.tv_nsec - start.tv_nsec;
        if (current < min && current > 0) {
            min = current;
            printf("Elapsed time in nanoseconds: %lf\n", (float) min / 2.0);
        }
    }

    return (EXIT_SUCCESS);
}
