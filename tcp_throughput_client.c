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
#define Mi Ki * Ki

#define MESSAGE_COUNT 10000
#define MESSAGE_SIZE ((long int) 512 * Ki)
#define RETURN_MESSAGE_SIZE 1
#define PORT 6969
#define NS_PER_S 1000000000

#define LOCALHOST "127.0.0.1"

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
    char message[MESSAGE_SIZE], return_message[RETURN_MESSAGE_SIZE];
    struct timespec start;
    struct timespec stop;
    long int min = __INT_MAX__;

    memset(message, 0, sizeof(message));
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Creating the TCP socket failed");
    }

    server.sin_addr.s_addr = inet_addr(LOCALHOST);
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
        for (int i = 0; i < MESSAGE_COUNT; i++) {
            write_until_full(sock, message);
        }
        if (read(sock, return_message, sizeof(return_message)) == -1) {
            perror("Reading return message failed");
        }
        // End clock
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
    }

    return (EXIT_SUCCESS);
}
