//
// Created by ori on 6/11/18.
//

#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <stdlib.h>

void getGameKey(int sig) {
    signal(SIGUSR2, getGameKey);

    char ch;
    ch = getchar();
    printf("Got: %c\n", ch);
    if (ch == 'q') {
        exit(0);
    }
}


/*int main() {
    printf("Entered to ex52.c\n");
    signal(SIGUSR2, getGameKey);
    while (1) {
        pause();
    }

}*/
