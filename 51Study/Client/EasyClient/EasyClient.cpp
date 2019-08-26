//
// Created by dendy on 19-8-5.
//


#include "EasyClient.h"

EasyClient::
EasyClient() {
    _message = new Message();
    _login = new Login();
    _ex = new Exit();

    memset(_hints, 0, sizeof(addrinfo));
}


EasyClient::
~EasyClient() {
    if (NULL != _message) {
        delete(_message);
        _message = NULL;
    }

    if (NULL != _login) {
        delete(_login);
        _login = NULL;
    }

    if (NULL != _ex) {
        delete(_ex);
        _ex = NULL;
    }

    if (NULL != _hints) {
        delete(_hints);
        _hints = NULL;
    }

    if (NULL != _username) {
        delete(&_username);
    }

    if (NULL != _password) {
        delete(&_password);
    }

    if (NULL != _recvBuf) {
        delete(&_recvBuf);
    }
}


Login* EasyClient::
getLogin() {
    _login->clear();
    return _login;
}


Exit* EasyClient::
getExit() {
    _ex->clear();
    return _ex;
}


Message* EasyClient::
getMessage() {
    _message->clear();
    return _message;
}


char* EasyClient::
getUsername() {
    return _username;
}


char* EasyClient::
getPassword() {
    return _password;
}


SOCKET EasyClient::
connect(char *ip, char* port) {
    addrinfo *servinfo, *p;
    int rv;
    SOCKET ret;

    memset(_hints, 0, sizeof(addrinfo));
    _hints->ai_family = AF_UNSPEC;
    _hints->ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(ip, port, _hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(1);
    }

    for (p = servinfo; p != NULL; p = p->ai_next) {
        initSocket(p);
        ret = ::connect(_socket, p->ai_addr, p->ai_addrlen);

        if (INVALID_SOCKET != ret) {
            break;
        }
    }

    return ret;
}


void EasyClient::
close() {
    if (INVALID_SOCKET != _socket) {
#ifdef _WIN32
        closesocket(_socket);
        WSACleanup();
#else
        ::close(_socket);
#endif
        _socket = INVALID_SOCKET;
    }
}


int EasyClient::
sendData(DataHeader *msg) {
    if (isRun() && msg) {
        int ret = ::send(_socket, (const char*)msg, msg->length, 0);

        if (SOCKET_ERROR == ret) {
            printf("ERROR: send message fail...\n");
        } else {
            std::cout << "INFO: send message success, " << getTypeName(msg);
        }

        return ret;
    }

    printf("ERROR: can't send message...\n");
    return SOCKET_ERROR;
}


bool EasyClient::isRun() {
    return _socket != INVALID_SOCKET;
}


void EasyClient::
initSocket(addrinfo *p) {
#ifdef _WIN32
    WORD ver = MAKEWORD(2, 2);
    WSADATA dat;
    WSAStartup(ver, &dat);
#endif

    if (INVALID_SOCKET != _socket) {
        printf("INFO: close old connection...\n");
        close();
    }

    _socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

    if (INVALID_SOCKET == _socket) {
        printf("ERROR: create socket fail...\n");
        return;
    }

    printf("INFO: create socket success...\n");

}

int EasyClient::
recvData() {
    memset(_recvBuf, 0, sizeof(_recvBuf));

    int nLen = (int)::recv(_socket, _recvBuf, sizeof(_recvBuf), 0);
    DataHeader *header = (DataHeader *)_recvBuf;


    if (nLen == 0) {
        printf("WARNING: disconnect from server...\n");
        return 0;
    }

    if (nLen < 0) {
        printf("ERROR: recv data fail...\n");
        return SOCKET_ERROR;
    }

    onNetMsg(header);
    return 0;
}


void EasyClient::
onNetMsg(DataHeader *msg) {
    switch (msg->cmd) {
        case CMD_LOGIN_RESULT: {
            LoginResult *loginResult = (LoginResult *) msg;
            printf("INFO: recv LoginResult success, result is : %d\n", loginResult->result);
            break;
        }
        case CMD_JOIN: {
            Join *join = (Join *) msg;
            printf("INFO: recv Join success, userName is : %s\n", join->userName);
            break;
        }
        case CMD_MESSAGE: {
            Message *message = (Message *) msg;
            printf("INFO: recv Message success, content is : %s\n", message->content);
            break;
        }
        case CMD_NOTIFY_EXIT: {
            NotifyExit *notifyExit = (NotifyExit *) msg;
            printf("INFO: recv NotifyExit success, userName is : %s\n", notifyExit->userName);
            break;
        }
        default:
            break;
    }
}

void EasyClient::
onRun() {
    recvData();
}