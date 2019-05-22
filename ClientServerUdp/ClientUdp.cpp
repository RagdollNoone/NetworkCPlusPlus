//
// Created by dendy on 19-5-21.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define PORT "50001"

#define MAXDATASIZE 100

void *
get_in_addr(sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((sockaddr_in *)sa)->sin_addr);
    }

    return  &(((sockaddr_in6 *)sa)->sin6_addr);
}

int
main(int argc, char *argv[]) {
    int sockfd, numbytes;
    char sendBuf[MAXDATASIZE];
    addrinfo hints, *servinfo;
    socklen_t sin_size;
    int rv;
    char s[INET6_ADDRSTRLEN];

    if (argc != 2) {
        fprintf(stderr, "usage: client hostname\n");
        exit(1);
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = 0;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(1);
    }

    if ((sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1) {
        perror("client socket");
        exit(1);
    }

    inet_ntop(servinfo->ai_family, get_in_addr(servinfo->ai_addr), s, sizeof(s));
    printf("client: connecting to %s\n", s);


    while(1) {
        printf("Input Message: ");
        scanf("%s", sendBuf);
        numbytes = sendto(sockfd, sendBuf, sizeof(sendBuf), 0, servinfo->ai_addr, sizeof(servinfo->ai_addr));
        printf("Send Message: %s, length is: %d\n", sendBuf, numbytes);
    }

    freeaddrinfo(servinfo);
    close(sockfd);

    return 0;
}