#include <QCoreApplication>
#include "cubotserver.h"
#include <QtDebug>
#include <signal.h>

QCoreApplication* a = NULL;

void int_handler(int signo) {
    if(signo == SIGINT)
        a->quit();
}


int main(int argc, char *argv[])
{
    signal(SIGINT, int_handler);
    a = new QCoreApplication(argc, argv);

    CuBotServer botServer;
    botServer.start();

    int ret = a->exec();

    botServer.stop();

    printf("\n\n\e[1;32m* \e[0mapplication exiting with result %d\n\n", ret);
}
