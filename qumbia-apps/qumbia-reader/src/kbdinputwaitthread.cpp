#include "kbdinputwaitthread.h"
#include <stdio.h>
#include <unistd.h>
#include <termios.h>

KbdInputWaitThread::KbdInputWaitThread(QObject *parent) : QThread(parent) {
}


void KbdInputWaitThread::run()
{
    char ch;
    struct termios old_tio, new_tio;
    /* get the terminal settings for stdin */
    tcgetattr(STDIN_FILENO,&old_tio);

    /* we want to keep the old setting to restore them a the end */
    new_tio=old_tio;

    /* disable canonical mode (buffered i/o) and local echo */
    new_tio.c_lflag &=(~ICANON & ~ECHO);

    /* set the new settings immediately */
    tcsetattr(STDIN_FILENO,TCSANOW,&new_tio);
    scanf("%c", &ch);

    /* restore the former settings */
    tcsetattr(STDIN_FILENO,TCSANOW,&old_tio);
}
