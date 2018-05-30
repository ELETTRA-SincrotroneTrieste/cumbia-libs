#include <TApplication>
#include "qtangoreader.h"
#include <X11/Xlib.h>
#include <QX11Info>


#define CVSVERSION "$Name:  $"


int main(int argc, char *argv[])
{
    int ret;
    TApplication qu_app( argc, argv );
    qu_app.setOrganizationName("-");
    qu_app.setApplicationName("-");
    QString version(CVSVERSION);
    qu_app.setApplicationVersion(version);
    qu_app.setProperty("author", "Giacomo");
    qu_app.setProperty("mail", "giacomo.strangolino@elettra.eu");
    qu_app.setProperty("phone", "-");
    qu_app.setProperty("office", "-");
    qu_app.setProperty("hwReferent", "-"); /* name of the referent that provides the device server */
    
    QTangoreader *w = new QTangoreader(NULL);


    ret = qu_app.exec();
    delete w;
    return ret;
}
