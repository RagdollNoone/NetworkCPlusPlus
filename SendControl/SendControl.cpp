//
// Created by dendy on 19-5-24.
//

#include <sys/socket.h>
#include <cstring>
#include <cstdio>
#include "SendControl.h"

int
sendAll(int socket, char *buf, int *len) {
    int sendLen = 0;
    int leftLen = *len;
    int n;

    while (sendLen < *len) {
        n = send(socket, buf + sendLen, leftLen, 0);

        if (n == -1) break;

        sendLen += n;
        leftLen -= n;

    }

    *len = sendLen;

    return n == -1 ? -1 : 0;
}

//void sendAllTest() {
//    char buf[10] = "Beej";
//    int len = strlen(buf);
//
//    // need a socket, maybe can use UDP here
//    if (sendAll(socket, buf, &len) == -1) {
//        perror("sendall");
//        printf("Only send %d bytes because of error", len);
//    }
//}