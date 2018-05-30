#include <quapplication.h>
#include "cumbiareader.h"
#include <X11/Xlib.h>
#include <QX11Info>

#include <cumbiapool.h>
#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>

#define CVSVERSION "$Name:  $"


int main(int argc, char *argv[])
{
    int ret;
    QuApplication qu_app( argc, argv );
    qu_app.setOrganizationName("-");
    qu_app.setApplicationName("-");
    QString version(CVSVERSION);
    qu_app.setApplicationVersion(version);
    qu_app.setProperty("author", "Giacomo");
    qu_app.setProperty("mail", "giacomo.strangolino@elettra.eu");
    qu_app.setProperty("phone", "-");
    qu_app.setProperty("office", "-");
    qu_app.setProperty("hwReferent", "-"); /* name of the referent that provides the device server */
    

    CumbiaPool *cu_p = new CumbiaPool();
    Cumbiareader *w = new Cumbiareader(cu_p, NULL);


    ret = qu_app.exec();
    delete w;
    delete cu_p->get("tango");
    delete cu_p->get("epics");
    return ret;
}
