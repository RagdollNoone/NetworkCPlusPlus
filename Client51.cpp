//
// Created by dendy on 19-5-28.
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
#include <thread>

#include "Message/Message.h"

#define PORT "3490"

#define MAXDATASIZE 100

Message *message = new Message();
Login *login = new Login();
Exit *ex = new Exit();

char recvBuf[1024];
char cmd[256];

int numbytes;

char userName[32];
char passWord[32];

void *
get_in_addr(sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((sockaddr_in *)sa)->sin_addr);
    }

    return  &(((sockaddr_in6 *)sa)->sin6_addr);
}

void
msgProcessor(int sockfd) {
    printf("do msgProcessor...\n");
    numbytes = recv(sockfd, (char *)&recvBuf, sizeof(recvBuf), 0);
    if (numbytes == -1) {

    } else {
        DataHeader *dh = (DataHeader *) recvBuf;
        switch (dh->cmd) {
            case CMD_LOGIN_RESULT: {
                LoginResult *loginResult = (LoginResult *) recvBuf;
                printf("Recv LoginResult success, result is : %d\n", loginResult->result);
                break;
            }
            case CMD_JOIN: {
                Join *join = (Join *) recvBuf;
                printf("Recv Join success, userName is : %s\n", join->userName);
                break;
            }
            case CMD_MESSAGE: {
                Message *message = (Message *) recvBuf;
                printf("Recv Message success, content is : %s\n", message->content);
                break;
            }
            case CMD_NOTIFY_EXIT: {
                NotifyExit *notifyExit = (NotifyExit *) recvBuf;
                printf("Recv NotifyExit success, userName is : %s\n", notifyExit->userName);
                break;
            }
            default:
                break;
        }
    }
}

void
scanProcessor(int sockfd, fd_set &master) {
    while(true) {
        printf("Please input the cmd: \n");
        scanf("%s", cmd);

        if (strncmp(cmd, "login", 5) == 0) {
            login->clear();
            strncpy(login->userName, userName, sizeof(login->userName));
            strncpy(login->passWord, passWord, sizeof(login->passWord));

            numbytes = send(sockfd, (char *) login, sizeof(Login), 0);
            if (numbytes == -1) {
                printf("Send Login fail\n");
            } else {
                printf("Send Login success, userName is : %s password is : %s\n", login->userName, login->passWord);
            }
        } else if (strncmp(cmd, "exit", 4) == 0) {
            ex->clear();
            strncpy(ex->userName, userName, sizeof(ex->userName));

            numbytes = send(sockfd, (char *) ex, sizeof(Exit), 0);
            if (numbytes == -1) {
                printf("Send Exit fail\n");
            } else {
                printf("Send Exit success, userName is : %s\n", ex->userName);
            }

            close(sockfd);
            FD_CLR(sockfd, &master);
            exit(0);
        } else {
            message->clear();
            strncpy(message->content, cmd, sizeof(message->content));

            numbytes = send(sockfd, (char *) message, sizeof(Message), 0);
            if (numbytes == -1) {
                printf("Send Message fail\n");
            } else {
                printf("Send Message success, content is : %s\n", message->content);
            }
        }
    }
}


int
main(int argc, char *argv[]) {
    int sockfd;
    char buf[MAXDATASIZE];
    addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    if (argc != 4) {
        fprintf(stderr, "usage: client hostname username and password\n");
        exit(1);
    }

    strncpy(userName, argv[2], sizeof(userName));
    strncpy(passWord, argv[3], sizeof(passWord));

    printf("userName is : %s\npassword is : %s\n", userName, passWord);

    fd_set master;
    fd_set branch;
    int fdmax;

    FD_ZERO(&master);
    FD_ZERO(&branch);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(2);
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
        exit(2);
    }

    inet_ntop(p->ai_family, get_in_addr((sockaddr *)p->ai_addr), s, sizeof(s));
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo);

    FD_SET(sockfd, &master);
    fdmax = sockfd;

    std::thread tScan(scanProcessor, sockfd, std::ref(master));

    int nready;
    while(true) {
        branch = master;
        nready = select(fdmax + 1, &branch, NULL, NULL, NULL);
        msgProcessor(sockfd);
    }

    close(sockfd);
    return 0;
}