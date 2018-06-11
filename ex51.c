#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>

#define STDERR_FD 2
#define TETRIS_PROG "./draw.out"

char getch() {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror ("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror ("tcsetattr ~ICANON");
    return (buf);
}

/**
 * prints error in sys call to stderr.
 */
void printErrorInSysCallToSTDERR() {
    char error_msg[] = "Error in system call\n";
    write(STDERR_FD, error_msg, sizeof(error_msg));
}

int isGameKey(char ch) {
    switch(ch) {
        case 'a':
        case 'd':
        case 's':
        case 'w':
        case 'q':
            return 1;
        default:
            return 0;
    }
}

int main() {
    int Pipe[2];
    pipe(Pipe);
    int pid;

    if ((pid = fork()) < 0) {
        printErrorInSysCallToSTDERR();
    }

    // Child process
    if (pid == 0) {
        printf("New process in town\n");
        // Force our stdin to be the read size of the pipe we made
        dup2(Pipe[0], 0);
        execlp(TETRIS_PROG, TETRIS_PROG, NULL);
        // Gets here only if exelp failed.
        printErrorInSysCallToSTDERR();
    }

    /* Father Process */
    char ch;
    while (1) {
        ch = getch();
        // char from stdin is not a game key.
        if (!isGameKey(ch)) {
            continue;
        }

        if(write(Pipe[1], &ch, 1) < 0) {
            printErrorInSysCallToSTDERR();
        }
        kill(pid, SIGUSR2);

        if (ch == 'q') {
            break;
        }
    }

    return 0;
}