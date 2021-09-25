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

#define MESSAGE_SIZE 63 * Ki + 995
#define PORT 6969

#define IP "128.105.37.156"

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in server, client;
    char message[MESSAGE_SIZE];
    struct timespec start;
    struct timespec stop;
    long int min = __INT_MAX__;

    memset(message, 0, sizeof(message));
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("Creating the UDP socket failed");
    }

    server.sin_addr.s_addr = inet_addr(IP);
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    int serverSize = sizeof(server);
    for (;;) {
        // Start clock
        if (clock_gettime(CLOCK_REALTIME, &start) == -1) {
            perror("Starting the clock failed");
        }
        // Respond
        if (sendto(sock, message, MESSAGE_SIZE, 0, (struct sockaddr*) &server, serverSize) == -1) {
            perror("Error sent");
        }
        
        // Recieve
        if (recvfrom(sock, message, MESSAGE_SIZE, 0, (struct sockaddr*) &server, &serverSize) == -1) {
            perror("Error recieved");
        }
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
