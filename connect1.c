/*
 * Purpose:
 *   Simple example program to demonstrate the use
 *   of the connect() syscall (blocking socket).
 *
 * Author:
 *   Christoph Lutz
 *
 * Date:
 *   Mar-09, 2024
 *
 * Compile:
 *   gcc -o connect1 connect1.c
 *
 * Usage:
 *  ./connect1 <hostname> <port>
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int err;
    int sockfd;
    struct addrinfo hints, *res;

    if(argc != 3) {
        printf("\nUsage: %s <hostname> <port>\n\n", argv[0]);
        return 1;
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = PF_UNSPEC;

    printf("Connecting to: %s port %d\n", argv[1], atoi(argv[2]));

    if((err = getaddrinfo(argv[1], argv[2], &hints, &res)) != 0) {
        printf("Error: getaddrinfo failed:  %s\n", gai_strerror(err));
        return 1;
    }

    if((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0) {
        printf("Error: socket failed\n");
        return 1;
    }

    #ifdef DEBUG
    printf("DEBUG: ip addr: %s\n", inet_ntoa(((struct sockaddr_in *)res->ai_addr)->sin_addr));
    printf("DEBUG: port: %d\n", htons(((struct sockaddr_in *)res->ai_addr)->sin_port));
    printf("DEBUG: sockfd=%d\n", sockfd);
    #endif

    if(connect(sockfd, res->ai_addr, res->ai_addrlen) < 0) {
        printf("Error: connect failed\n");
        return 1;
    }

    printf("Connected.\n");

    close(sockfd);
    freeaddrinfo(res);

    return 0;
}
