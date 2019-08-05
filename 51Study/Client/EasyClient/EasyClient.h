//
// Created by dendy on 19-8-5.
//

#ifndef NETWORK_EASYCLIENT_H
#define NETWORK_EASYCLIENT_H


#include "../../../Message/Message.h"


#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <cstdlib>
#include <sys/socket.h>
#endif

#include <stdio.h>
#include <string>

#define SOCKET int
#define INVALID_SOCKET (SOCKET)(~0)
#define SOCKET_ERROR (-1)

class EasyClient {
public:
    EasyClient();
    virtual ~EasyClient();

    SOCKET connect(char* , char*);
    void close();
    int sendData(DataHeader *);

    bool isRun();

    char *getUsername();
    char *getPassword();

    void setUsername();
    void setPassword();

    Login *getLogin();
    Exit *getExit();
    Message *getMessage();

private:
    void initSocket(addrinfo *);

    int recvData();

    void onRun();
    void onNetMsg(DataHeader *);

private:
    SOCKET _socket;
    addrinfo *_hints;

    Message *_message;
    Login *_login;
    Exit *_ex;

    char _username[32];
    char _password[32];

    char _recvBuf[4096];
};


#endif //NETWORK_EASYCLIENT_H
