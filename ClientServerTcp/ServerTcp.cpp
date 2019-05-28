//
// Created by dendy on 19-5-20.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#include "../Message/Message.h"

#define PORT "3490"

#define BACKLOG 10

void
sigchild_handler(int s) {
    (void)s; // TODO: 这句话啥意思

    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}


void *
get_in_addr(sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((sockaddr_in *)sa)->sin_addr);
    }

    return &(((sockaddr_in6 *)sa)->sin6_addr);
}

int
main(void) {
    int sockfd, new_fd, nbytes;
    addrinfo hints, *servinfo, *p;
    sockaddr_storage their_addr;
    socklen_t sin_size;
    struct sigaction sa;
    int yes = 1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    for (p = servinfo; p != NULL; p = p->ai_next) {
        // TODO: 指定了socketype为什么还要指定protocol
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("server socket");
            continue;
        }

        // 处理端口被占用的情况
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1); // TODO: exit(1)和return 1有什么区别
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo);

    if (NULL == p) {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchild_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");

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

    sin_size = sizeof(their_addr);
    new_fd = accept(sockfd, (sockaddr *)&their_addr, &sin_size);
    if (new_fd == -1) {
        perror("accept");
    }

    inet_ntop(their_addr.ss_family, get_in_addr((sockaddr *)&their_addr), s, sizeof(s));
    printf("server: got connection from %s\n", s);

    while(true) {
        if ((nbytes = recv(new_fd, (char *)&dh, sizeof(DataHeader), 0)) == -1) {
            printf("Recv DataHeader fail\n");
        } else {
            if (dh.cmd == CMD_LOGIN) {

                if ((nbytes = recv(new_fd, (char *)&login, sizeof(Login), 0)) == -1) {
                    printf("Recv Login fail\n");
                    loginResult.result = 1;
                } else {
                    printf("Recv Login success, userName is : %s password is : %s\n", login.userName, login.password);
                    loginResult.result = 0;
                }

                if ((nbytes = send(new_fd, (char *)&loginResult, sizeof(loginResult), 0)) == -1) {
                    printf("Send LoginResult fail\n");
                } else {
                    printf("Send LoginResult success, result is : %d\n", loginResult.result);
                }

            } else if (dh.cmd == CMD_LOGOUT) {

                if ((nbytes = recv(new_fd, (char *)&logout, sizeof(Logout), 0)) == -1) {
                    printf("Recv Logout fail\n");
                    logoutResult.result = 1;
                } else {
                    printf("Recv Logout success, userName is : %s\n", logout.userName);
                    logoutResult.result = 0;
                }

                if ((nbytes = send(new_fd, (char *)&logoutResult, sizeof(logoutResult), 0)) == -1) {
                    printf("Send LogoutResult fail\n");
                } else {
                    printf("Send LogoutResult success, result is : %d\n", logoutResult.result);
                }

            }
        }

    }

    return 0;
}