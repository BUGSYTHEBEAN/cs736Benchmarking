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

#define PARENT_SOCKET 0
#define CHILD_SOCKET 1

#define MESSAGE_SIZE 4

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
        perror("Creating the socketpair failed");
    }

    server.sin_port = htons(2000);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (struct sockaddr*)&server, sizeof(server)) == -1){
        perror("Binding socket fail.");
    }

    // binding worked start looking for the transfers
    int clientSize = sizeof(client);
    for(;;){    // loop
        //recieve
        if (recvfrom(sock, message, MESSAGE_SIZE, 0, (struct sockaddr*)&client, &clientSize) == -1){
            perror("error recieve");
        }
        //respond
        if (sendto(sock, message, MESSAGE_SIZE, 0, (struct sockaddr*)&client, clientSize) == -1){
            perror("error send");
        }
    }

    return (EXIT_SUCCESS);
}