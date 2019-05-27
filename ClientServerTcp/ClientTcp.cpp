//
// Created by dendy on 19-5-20.
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

#include "../Message/Message.h"

#define PORT "3490"

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
    char buf[MAXDATASIZE];
    addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    char userName[32];
    char passWord[32];
    char cmd[32];


    if (argc != 4) {
        fprintf(stderr, "usage: client hostname username and password\n");
        exit(1);
    }

    strncpy(userName, argv[2], sizeof(userName));
    strncpy(passWord, argv[3], sizeof(passWord));

    printf("userName is : %s\npassword is : %s\n", userName, passWord);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            perror("client: connect");
            close(sockfd);
            continue;
        }

        break;
    }

    if (NULL == p) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((sockaddr *)p->ai_addr), s, sizeof(s));

    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo);

    DataHeader dh;
    memset(&dh, 0, sizeof(DataHeader));

    Login login;
    memset(&login, 0, sizeof(Login));

    LoginResult loginResult;
    memset(&loginResult, 0, sizeof(LoginResult));

    Logout logout;
    memset(&logout, 0, sizeof(Logout));

    LogoutResult logoutResult;
    memset(&logoutResult, 0, sizeof(logoutResult));

    while(true) {
        printf("Please input the cmd: ");
        scanf("%s", cmd);

       if (strncmp(cmd, "login", 5)) {
           dh.cmd = CMD_LOGIN;
           dh.length = sizeof(Login);

           numbytes = send(sockfd, (char *)&dh, sizeof(DataHeader), 0);
           if (numbytes == -1) {
               printf("Send Login DataHeader fail");
           } else {
               printf("Send Login DataHeader success,\ncmd value is : %d\nlength is : %d\n", dh.cmd, dh.length);
           }

           strncpy(login.userName, userName, sizeof(login.userName));
           strncpy(login.password, passWord, sizeof(login.password));

           numbytes = send(sockfd, (char *)&login, sizeof(Login), 0);
           if (numbytes == -1) {
               printf("Send Login fail");
           } else {
               printf("Send Login success, \nuserName is : %s\npassword is : %s\n", login.userName, login.password);
           }

           numbytes = recv(sockfd, (char *)&loginResult, sizeof(LoginResult), 0);
           if (numbytes == -1) {
               printf("Recv LoginResult fail");
           } else {
               printf("Recv LoginResult success, \nresult is : %d\n", loginResult.result);
           }


       } else if (strncmp(cmd, "logout", 6) == 0) {
           dh.cmd = CMD_LOGOUT;
           dh.length = sizeof(Logout);

           numbytes = send(sockfd, (char *)&dh, sizeof(DataHeader), 0);
           if (numbytes == -1) {
               printf("Send Logout DataHeader fail");
           } else {
               printf("Send Logout DataHeader success, \ncmd value is : %d\nlength is : %d\n", dh.cmd, dh.length);
           }

           strncpy(logout.userName, userName, sizeof(logout.userName));

           numbytes = send(sockfd, (char *)&logout, sizeof(Logout), 0);
           if (numbytes == -1) {
               printf("Send Logout fail");
           } else {
               printf("Send Logout success, \nuserName is : %s\n", logout.userName);
           }

           numbytes = recv(sockfd, (char *)&logoutResult, sizeof(LogoutResult), 0);
           if (numbytes == -1) {
               printf("Recv LogoutResult fail");
           } else {
               printf("Recv LogoutResult success, \nresult is : %d\n", logoutResult.result);
           }

       } else {
           printf("Please input login or logout");

       }
    }

    close(sockfd);
    return 0;
}