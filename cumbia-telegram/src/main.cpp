#include <QApplication>
#include "cubotserver.h"
#include <QtDebug>
#include <QTimer>
#include <signal.h>

static QTimer *timer = nullptr;
static QTimer *server_start_tmr = nullptr;
static QTimer *server_stop_tmr = nullptr;

void int_handler(int signo) {
    if(signo == SIGINT)
        timer->start(1);
    else if(signo == SIGUSR1)
        server_start_tmr->start(1);
    else if(signo == SIGUSR2)
        server_stop_tmr->start(1);
}


int main(int argc, char *argv[])
{
    signal(SIGINT, int_handler);
    signal(SIGUSR1, int_handler);
    signal(SIGUSR2, int_handler);
    QApplication *a = new QApplication(argc, argv);

    CuBotServer botServer;
    botServer.start();

    // manage termination and start/stop signals
    timer = new QTimer(a);
    QObject::connect(timer, SIGNAL(timeout()), a, SLOT(quit()));

    server_start_tmr = new QTimer(a);
    QObject::connect(server_start_tmr, SIGNAL(timeout()), &botServer, SLOT(start()));
    server_stop_tmr = new QTimer(a);
    QObject::connect(server_stop_tmr, SIGNAL(timeout()), &botServer, SLOT(stop()));
    foreach(QTimer *t, a->findChildren<QTimer *>())
        t->setSingleShot(true);
    int ret = a->exec();

    botServer.stop();

    printf("\n\n\e[1;32m* \e[0mapplication exiting with result %d\n\n", ret);
}
