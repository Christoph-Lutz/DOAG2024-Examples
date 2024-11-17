/*
 * Purpose:
 *   Simple example program to demonstrate the use
 *   of the connect() syscall with a non-blocking
 *   socket.
 *
 * Author:
 *   Christoph Lutz
 *
 * Date:
 *   Mar-09, 2024
 *
 * Compile:
 *   gcc -o connect2 connect2.c
 *
 * Usage:
 *  ./connect2 <hostname> <port>
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
#include <errno.h>
#include <fcntl.h>
#include <poll.h>

int main(int argc, char *argv[]) {
    int err;
    int sockfd, sockfd_ready;
    struct addrinfo hints, *res;
    struct pollfd pfds[1];

    if(argc != 4) {
        printf("\nUsage: %s <hostname> <port> <timeout ms>\n\n", argv[0]);
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

    if((fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL, 0) | O_NONBLOCK)) == -1) {
        printf("Error: fcntl\n");
        return 1;
    }   

    #ifdef DEBUG
    printf("DEBUG: ip addr: %s\n", inet_ntoa(((struct sockaddr_in *)res->ai_addr)->sin_addr));
    printf("DEBUG: port: %d\n", htons(((struct sockaddr_in *)res->ai_addr)->sin_port));
    printf("DEBUG: timeout: %d\n", atoi(argv[3]));
    printf("DEBUG: sockfd=%d\n", sockfd);
    #endif

    pfds[0].fd = sockfd;
    pfds[0].events = POLLOUT;

    if(connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
        if(errno == EINPROGRESS) {
            sockfd_ready = poll(pfds, 1, atoi(argv[3]));
            
            if(sockfd_ready > 0 && pfds[0].revents & POLLOUT) {
                printf("Connection is ready.\n");
            }
            else if (sockfd_ready == 0) {
                printf("Connection timed out.\n");
            }
            else {
                printf("Connect failed. Aborting.\n");
                return 1;
            }
        }
    }

    printf("Connected.\n");

    close(sockfd);
    freeaddrinfo(res);

    return 0;
}
