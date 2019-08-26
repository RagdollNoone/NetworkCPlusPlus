//
// Created by dendy on 19-8-5.
//

#include "EasyClient.h"

struct localCache{
    char cmd[32];
    Message *msg = new Message();
};

localCache *cache = new localCache();

void
cmdThread(EasyClient *client) {
    while(true) {
        printf("Please input the cmd: \n");
        scanf("%s", cache->cmd);

        if (strncmp(cache->cmd, "login", 5) == 0) {
            Login *login = client->getLogin();
            strncpy(login->userName, client->getUsername(), sizeof(login->userName));
            strncpy(login->passWord, client->getPassword(), sizeof(login->passWord));
            client->sendData(login);
        } else if (strncmp(cache->cmd, "exit", 4) == 0) {
            Exit *ex = new Exit();
            strncpy(ex->userName, client->getUsername(), sizeof(ex->userName));
            client->sendData(ex);
            client->close();
        } else {
            Message *msg = cache->msg;
            msg->clear();
            strncpy(msg->content, cache->cmd, sizeof(msg->content));
            client->sendData(msg);
        }
    }
}


int
main(int argc, char *argv[]) {

}