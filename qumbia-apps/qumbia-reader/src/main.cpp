#include <QCoreApplication>
#include "qumbia-reader.h"

#include <cumbiapool.h>
#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>
#include <signal.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>

#define VERSION "1.0.0"

// terminal settings saved in main.cpp
// and restored in signal handler, just in
// case the KbdInputWaitThread can't do it
// or the application is terminated with Ctrl-C
//
struct termios m_default_tio;

void signal_handler(int signo) {
    if(signo == SIGINT)
        printf("\e[1;31m * \e[0m: qumbia-reader: press any key to stop monitor instead of CTRL-C");

    /* restore the terminal settings, if modified by KbdInputWaitThread */
    tcsetattr(STDIN_FILENO,TCSANOW,&m_default_tio);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    int ret = 0;
    /* get the terminal settings for stdin */
    tcgetattr(STDIN_FILENO,& m_default_tio);

    signal(SIGINT, signal_handler);
    QCoreApplication qu_app( argc, argv );
    QString version(VERSION);
    qu_app.setApplicationVersion(version);

    CumbiaPool *cu_p = new CumbiaPool();
    QumbiaReader *w = new QumbiaReader(cu_p, NULL);
    if(!w->usage_only() && !w->config_error())
        ret = qu_app.exec();
    delete w;
    if(cu_p->get("tango"))
        delete cu_p->get("tango");
    if(cu_p->get("epics"))
        delete cu_p->get("epics");
    if(cu_p->get("random"))
        delete cu_p->get("random");
    return ret;
}
