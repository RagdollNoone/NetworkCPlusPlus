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
NotifyExit *notifyExit = new NotifyExit();

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
selectProcessor(int new_fd, fd_set &master, const int fdmax) {
    int nbytes = recv(new_fd, (char *)&recvBuf, sizeof(recvBuf), 0);
    if (nbytes < 0) {
        printf("Server recv error\n");
    } else if (nbytes == 0) {
        printf("A Client disconnect\n");
        selectClose(new_fd, master);
    } else {
        DataHeader *dh = (DataHeader *)recvBuf;

        switch (dh->cmd) {
            case CMD_LOGIN: {
                // 接收登陆消息
                Login *login = (Login *) recvBuf;
                printf("Recv Login success, userName is : %s password is : %s\n", login->userName, login->passWord);

                // 发送回包给客户端
                loginResult->result = 0;
                nbytes = send(new_fd, (char *) loginResult, sizeof(LoginResult), 0);
                nbytes == -1 ? printf("Send LoginResult fail\n" ) : printf("Send LoginResult success\n" );

                // 发送登陆消息给其他客户端
                for (int i = 0; i <= fdmax; i++) {
                    if (FD_ISSET(i, &master) && i != new_fd && i != listenfd) {
                        strcpy(join->userName, login->userName);
                        nbytes = send(i, (char *) join, sizeof(Join), 0);
                        nbytes == -1 ? printf("Send Join fail\n" ) : printf("Send Join success\n" );
                    }
                }

                break;
            }
            case CMD_MESSAGE: {
                // 接收消息内容
                Message *recvMsg = (Message *) recvBuf;
                printf("Recv Message success, content is : %s\n", recvMsg->content);

                // 发送给其他客户端
                for (int i = 0; i <= fdmax; i++) {
                    if (FD_ISSET(i, &master) && i != new_fd && i != listenfd) {
                        message->clear();
                        strcpy(message->content, recvMsg->content);
                        nbytes = send(i, (char *) message, sizeof(Message), 0);
                        nbytes == -1 ? printf("Send Message fail\n" ) : printf("Send Message success\n" );
                    }
                }

                break;
            }
            case CMD_EXIT: {
                // 接收退出消息
                Exit *exit = (Exit *) recvBuf;
                printf("Recv Exit success, userName is : %s\n", exit->userName);

                selectClose(new_fd, master);

                // 发送给其他客户端
                for (int i = 0; i <= fdmax; i++) {
                    if (FD_ISSET(i, &master) && i != new_fd && i != listenfd) {
                        notifyExit->clear();
                        strcpy(notifyExit->userName, exit->userName);
                        nbytes = send(i, (char *) notifyExit, sizeof(NotifyExit), 0);
                        nbytes == -1 ? printf("Send Message fail\n" ) : printf("Send Message success\n" );
                    }
                }

                break;
            }
            default:
                break;
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
    fd_set branch;
    int fdmax;

    FD_ZERO(&master);
    FD_ZERO(&branch);

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
        if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
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

    FD_SET(listenfd, &master);
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
        branch = master;
        int nready = select(fdmax + 1, &branch, NULL, NULL, NULL);
        if (nready == -1) {
            perror("select listen");
            exit(5);
        }

        for (int i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &branch) ) {
                if (i != listenfd) {
                    selectProcessor(i, master, fdmax);
                } else {
                    selectAccept(listenfd, master, fdmax);
                }
            }
        }
    }

    return 0;
}