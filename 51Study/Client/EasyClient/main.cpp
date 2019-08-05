//
// Created by dendy on 19-8-5.
//

#include "EasyClient.h"

void
cmdThread(EasyClient *client) {
    while(true) {
        printf("Please input the cmd: \n");
        scanf("%s", cmd);

        if (strncmp(cmd, "login", 5) == 0) {
            Login *login = client->getLogin();
            strncpy(login->userName, client->getUsername(), sizeof(login->userName));
            strncpy(login->passWord, client->getPassword(), sizeof(login->passWord));
            client->sendData(login);

//            numbytes = send(sockfd, (char *) login, sizeof(Login), 0);
//            if (numbytes == -1) {
//                printf("Send Login fail\n");
//            } else {
//                printf("Send Login success, userName is : %s _password is : %s\n", login->userName, login->passWord);
//            }
        }
//        else if (strncmp(cmd, "exit", 4) == 0) {
//            ex->clear();
//            strncpy(ex->userName, userName, sizeof(ex->userName));
//
//            numbytes = send(sockfd, (char *) ex, sizeof(Exit), 0);
//            if (numbytes == -1) {
//                printf("Send Exit fail\n");
//            } else {
//                printf("Send Exit success, userName is : %s\n", ex->userName);
//            }
//
//            close(sockfd);
//            FD_CLR(sockfd, &master);
//            exit(0);
//        } else {
//            message->clear();
//            strncpy(message->content, cmd, sizeof(message->content));
//
//            numbytes = send(sockfd, (char *) message, sizeof(Message), 0);
//            if (numbytes == -1) {
//                printf("Send Message fail\n");
//            } else {
//                printf("Send Message success, content is : %s\n", message->content);
//            }
//        }
    }
}


int
main(int argc, char *argv[]) {

}