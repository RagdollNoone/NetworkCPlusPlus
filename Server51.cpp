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

#include "Message/Message.h"

#define PORT "3490"

#define BACKLOG 10

LoginResult *loginResult = new LoginResult;
Join *join = new Join();
Message *message = new Message();

char recvBuf[1024];
char remoteIP[INET6_ADDRSTRLEN];
int listenfd;


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
selectAccept(int sockfd, fd_set &master, int &fdmax) {
    sockaddr_storage remoteaddr;
    socklen_t addrlen = sizeof(sockaddr_storage);

    int new_fd = accept(sockfd, (sockaddr *)&remoteaddr, &addrlen);

    if (-1 == new_fd) {
        perror("accept");
    } else {
        FD_SET(new_fd, &master);

        if (new_fd > fdmax) {
            fdmax = new_fd;
        }

        printf("selectserver: new connection from %s on socket %d\n", inet_ntop(remoteaddr.ss_family, get_in_addr((sockaddr *)&remoteaddr), remoteIP, INET6_ADDRSTRLEN), new_fd);
    }


    return new_fd;
}

void
selectClose(int sockfd, fd_set &master) {
    close(sockfd);
    FD_CLR(sockfd, &master);
}

void
selectProcessor(int new_fd, fd_set &master) {
    int nbytes;
    if ((nbytes = recv(new_fd, (char *)&recvBuf, sizeof(recvBuf), 0)) == -1) {
        printf("Analysis DataHeader fail\n");
    } else {
        DataHeader *dh = (DataHeader *)recvBuf;

        switch (dh->cmd) {
            case CMD_LOGIN:
                Login *login = (Login *)recvBuf;
                printf("Recv Login success, userName is : %s password is : %s\n", login.userName, login.passWord);

                loginResult->result = 0;
                send(new_fd, (char *)login, sizeof(LoginResult), 0);




                break;
            case CMD_MESSAGE:
                break;
            case CMD_EXIT:
                break;
            default:
                break;
        }





        if (dh.cmd == CMD_LOGIN) {
            nbytes = recv(new_fd, (char *)&login, sizeof(Login), 0);

            if (nbytes == -1) {
                printf("Recv Login fail\n");
                loginResult.result = 1;
            } else if (nbytes == 0) {
                selectClose(new_fd, master);
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
            nbytes = recv(new_fd, (char *)&logout, sizeof(Logout), 0);
            if (nbytes == -1) {
                printf("Recv Logout fail\n");
                logoutResult.result = 1;
            } else if (nbytes == 0) {
                selectClose(new_fd, master);
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

int
main(void) {
    addrinfo hints, *servinfo, *p;
    struct sigaction sa;
    int yes = 1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    fd_set master;
    fd_set listen_fds;
    fd_set process_fds;
    int fdmax;

    FD_ZERO(&master);
    FD_ZERO(&listen_fds);
    FD_ZERO(&process_fds);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(1);
    }

    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("server socket");
            continue;
        }

        // 处理端口被占用的情况
        if (setsockopt(, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("setsockopt");
            exit(2);
        }

        if (bind(listenfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(listenfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo);

    if (NULL == p) {
        fprintf(stderr, "server: failed to bind\n");
        exit(3);
    }

    if (listen(listenfd, BACKLOG) == -1) {
        perror("listen");
        exit(4);
    }

    FD_SET(listenfd, &listen_fds);
    fdmax = listenfd;

    sa.sa_handler = sigchild_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(5);
    }

    printf("server: waiting for connections...\n");

    while(true) {
        timeval t = {1, 0};

        master = process_fds;
        int nready = select(fdmax + 1, &master, NULL, NULL, &t);
        if (nready == -1) {
            perror("select process");
            exit(5);
        }

        for (int i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &master)) {
                selectProcessor(i, master);
            }
        }

        master = listen_fds;
        nready = select(fdmax + 1, &master, NULL, NULL, &t);
        if (nready == -1) {
            perror("select listen");
            exit(6);
        }

        selectAccept(listenfd, master, fdmax);
    }

    return 0;
}