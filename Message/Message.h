//
// Created by shzhou on 2019/5/27.
//

#ifndef NETWORK_MESSAGE_H
#define NETWORK_MESSAGE_H

#include <iostream>
#include <string>
#include <cstring>

enum CMD {
    CMD_LOGIN = 0,
    CMD_LOGIN_RESULT = 1,
    CMD_JOIN = 2,
    CMD_MESSAGE = 3,
    CMD_EXIT = 4,
    CMD_NOTIFY_EXIT = 5,
};

struct DataHeader {
    int length;
    int cmd;
};

struct Login : public DataHeader{
public:
    Login() {
        clear();
    }

    void clear() {
        cmd = CMD_LOGIN;
        length = sizeof(Login);
        memset(userName, 0, sizeof(userName));
        memset(passWord, 0, sizeof(passWord));
    }

    char userName[32];
    char passWord[32];
};

struct LoginResult : public DataHeader {
public:
    LoginResult() {
        clear();
    }

    void clear() {
        cmd = CMD_LOGIN_RESULT;
        length = sizeof(LoginResult);
        result = -1;
    }

    int result;
};

struct Join : public DataHeader {
public:
    Join() {
        clear();
    }

    void clear() {
        cmd = CMD_JOIN;
        length = sizeof(Join);
        memset(userName, 0, sizeof(userName));
    }

    char userName[32];
};

struct Message : public DataHeader {
public:
    Message() {
        clear();
    }

    void clear() {
        cmd = CMD_MESSAGE;
        length = sizeof(Message);
        memset(content, 0, sizeof(content));
    }

    char content[256];
};

struct Exit : public DataHeader {
public:
    Exit() {
        clear();
    }

    void clear() {
        cmd = CMD_EXIT;
        length = sizeof(Exit);
        memset(userName, 0, sizeof(userName));
    }

    char userName[32];
};

struct NotifyExit : public DataHeader {
public:
    NotifyExit() {
        clear();
    }

    void clear() {
        cmd = CMD_NOTIFY_EXIT;
        length = sizeof(NotifyExit);
        memset(userName, 0, sizeof(userName));
    }

    char userName[32];
};

static std::string getTypeName(DataHeader *msg) {
    std::string typeName = "";

    switch (msg->cmd) {
        case 0: {
            typeName = "Login";
            break;
        }
        case 1: {
            typeName = "LoginResult";
            break;
        }
        case 2: {
            typeName = "Join";
            break;
        }
        case 3: {
            typeName = "Message";
            break;
        }
        case 4: {
            typeName = "Exit";
            break;
        }
        case 5: {
            typeName = "NotifyExit";
            break;
        }
        default:
            break;
    }

    return typeName;
}
#endif //NETWORK_MESSAGE_H
