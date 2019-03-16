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
    QString tok, dbfile;
    foreach(QString s, qApp->arguments()) {
        if(s.startsWith("--token="))
            tok = s.remove("--token=");
        if(s.startsWith("--db="))
            dbfile = s.remove("--db=");
    }
    if(tok.isEmpty() || dbfile.isEmpty()) {
        printf("\e[1;32;4mUSAGE\e[0m: %s    --token=\e[1;36mbot_token\e[0m --db=\e[1;36mpath/to/sqlite_db.dat\e[0m\n\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    CuBotServer botServer(a, tok, dbfile);
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
