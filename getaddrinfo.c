/*
 * Purpose:
 *   Simple example program to demonstrate the
 *   use of the libc getaddrinfo() function.
 *
 * Author:
 *   Christoph Lutz
 *
 * Date:
 *   Mar-09, 2024
 *
 * Compile:
 *   gcc -o getaddrinfo getaddrinfo.c
 *
 * Usage:
 *  ./getaddrinfo <hostname> 
 *
 */

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {

    struct addrinfo hints, *res, *result;
    void *ptr;
    char addrstr[INET_ADDRSTRLEN];
    int err;

    if(argc != 2) {
        printf("\nUsage: %s <hostname>\n\n", argv[0]);
        return 1;
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags |= AI_CANONNAME;

    err = getaddrinfo(argv[1], NULL, &hints, &result);

    if (err != 0) {
      printf("Error in getaddrinfo: %s\n", gai_strerror(err));
      exit(EXIT_FAILURE);
    }

    for (res = result; res != NULL; res = res->ai_next) {

        switch(res->ai_family) {
            case AF_INET:
                ptr = &((struct sockaddr_in *) res->ai_addr)->sin_addr;
                break;
            case AF_INET6:
                ptr = &((struct sockaddr_in6 *) res->ai_addr)->sin6_addr;
                break;
            }

            inet_ntop (res->ai_family, ptr, addrstr, INET_ADDRSTRLEN);
            printf("IPv%d address: %s (%s)\n",  res->ai_family == PF_INET6 ? 6 : 4,
                   addrstr, res->ai_canonname);
    }

    freeaddrinfo(result);
    return 0;
}

