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

    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    if (bind(sock, (struct sockaddr*) &server, sizeof(server)) == -1) {
        perror("Binding socket failed");
    }

    // Binding worked—start looking for the transfers
    int clientSize = sizeof(client);
    for (;;) {
        // Recieve
        if (recvfrom(sock, message, MESSAGE_SIZE, 0, (struct sockaddr*) &client, &clientSize) == -1) {
            perror("error recieve");
        }
        // Respond
        if (sendto(sock, message, MESSAGE_SIZE, 0, (struct sockaddr*) &client, clientSize) == -1) {
            perror("error send");
        }
    }

    return (EXIT_SUCCESS);
}
