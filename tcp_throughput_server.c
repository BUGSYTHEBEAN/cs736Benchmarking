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

#define BACKLOG 10
#define MESSAGE_COUNT 10000
#define MESSAGE_SIZE ((long int) 512 * Ki)
#define RETURN_MESSAGE_SIZE 1
#define PORT 6969
#define NS_PER_S 1000000000

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

int main(int argc, char *argv[]) {
    int connection, sock;
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

    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    if (bind(sock, (struct sockaddr*) &server, sizeof(server)) == -1) {
        perror("Binding socket failed");
    }

    if ((listen(sock, BACKLOG)) == -1) {
        perror("Listen failed");
    }

    int clientSize = sizeof(client);
    connection = accept(sock, (struct sockaddr*) &client, &clientSize);
    if (connection == -1) {
        perror("Accepting connection failed");
    }

    for (;;) {
        for (int i = 0; i < MESSAGE_COUNT; i++) {
            read_until_full(connection, message);
        }
        if (write(connection, return_message, sizeof(return_message)) == -1) {
            perror("Writing return message failed");
        }
    }

    return (EXIT_SUCCESS);
}
