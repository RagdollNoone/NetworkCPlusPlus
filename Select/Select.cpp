//
// Created by dendy on 19-5-22.
//

#include <sys/time.h>
#include <sys/types.h>
#include <stdio.h>

#define STDIN 0

int
main(void) {
    timeval tv;
    fd_set readfds;

    tv.tv_sec = 2;
    tv.tv_usec = 500000;

    FD_ZERO(&readfds);
    FD_SET(STDIN, &readfds);

    select(STDIN + 1, &readfds, NULL, NULL, &tv);

    if (FD_ISSET(STDIN, &readfds))
        printf("A key was pressed!\n");
    else
        printf("Timed out.\n");

    return 0;
}