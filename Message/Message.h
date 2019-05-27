//
// Created by shzhou on 2019/5/27.
//

#ifndef NETWORK_MESSAGE_H
#define NETWORK_MESSAGE_H

enum CMD {
    CMD_LOGIN,
    CMD_LOGOUT
};

struct DataHeader {
    int length;
    int cmd;
};

struct Login {
    char userName[32];
    char password[32];
};

struct LoginResult {
    int result;
};

struct Logout {
    char userName[32];
};

struct LogoutRsult {
    int result;
};

#endif //NETWORK_MESSAGE_H
