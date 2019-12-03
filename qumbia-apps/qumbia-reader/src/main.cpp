#include <QCoreApplication>
#include "qumbia-reader.h"

#include <cumbiapool.h>
#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>
#include <signal.h>

#define CVSVERSION "$Name:  $"


void signal_handler(int signo) {
    if(signo == SIGINT)
        printf("\e[1;31m * \e[0m: qumbia-reader: press any key to stop monitor instead of CTRL-C");
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    int ret = 0;
    signal(SIGINT, signal_handler);
    QCoreApplication qu_app( argc, argv );
    QString version(CVSVERSION);
    qu_app.setApplicationVersion(version);

    CumbiaPool *cu_p = new CumbiaPool();
    QumbiaReader *w = new QumbiaReader(cu_p, NULL);
    if(!w->usage_only())
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
