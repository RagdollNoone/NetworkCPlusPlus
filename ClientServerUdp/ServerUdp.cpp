//
// Created by dendy on 19-5-21.
//

#include <sys/socket.h>
#include <cstdio>
#include <cstdlib>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT "50001"

void *
get_in_addr(sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((sockaddr_in *)sa)->sin_addr);
    }

    return &(((sockaddr_in6 *)sa)->sin6_addr);
}

int
main(int argc, char *argv[]) {
    int sockfd, numbytes;
    addrinfo hints, *servinfo;
    socklen_t sin_size;
    char recvBuf[549];
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = 0;

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(1);
    }

    if ((sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1) {
        perror("server socket");
        exit(1);
    }

    if (bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
        close(sockfd);
        perror("server: bind");
        exit(1);
    }

    printf("server: waiting for connections...\n");

//    while(1) {
        printf("in server while");
        sin_size = sizeof(servinfo->ai_addr);
        numbytes = recvfrom(sockfd, recvBuf, sizeof(recvBuf) - 1, 0, (servinfo->ai_addr), &sin_size);

        printf("numbytes: %d", numbytes);
        inet_ntop(servinfo->ai_family, servinfo->ai_addr, s, sizeof(s));
        printf("server: got data from %s\n", s);

        if (numbytes < sizeof(recvBuf)) {
            recvBuf[numbytes] = '\0';
        }

        printf("content: %s\n", recvBuf);
//    }

    freeaddrinfo(servinfo);
    close(sockfd);

    return 0;
}