#include <quapplication.h>
#include "cumparsita.h"
#include <QtDebug>

#include <X11/Xlib.h>
#include <QX11Info>
#include <QMessageBox>

#define CVSVERSION "$Name:  $"


int main(int argc, char *argv[])
{
    int ret;
    QuApplication *qu_app = new QuApplication( argc, argv );
    qu_app->setOrganizationName("elettra");
    qu_app->setApplicationName("cumparsita");
    QString version(CVSVERSION);
    qu_app->setApplicationVersion(version);
    qu_app->setProperty("author", "Giacomo");
    qu_app->setProperty("mail", "giacomo.strangolino@elettra.eu");
    qu_app->setProperty("phone", "0403758073");
    qu_app->setProperty("office", "T2PT025");
    qu_app->setProperty("hwReferent", "$HW_REFERENT$"); /* name of the referent that provides the device server */

    if(qApp->arguments().size() < 2) {
        QMessageBox::information(nullptr, "Usage", QString("%1 path/to/file.ui").arg(qApp->arguments().first()));
        ret = EXIT_SUCCESS;
    }
    else {

        Cumparsita *w = new Cumparsita(nullptr);
        //
        // get reference to cumbia designer plugin to invoke cumbia_tango_free at the proper
        // moment (before the application destroys CuCustomWidgetCollectionInterface causing a
        // crash in Tango while deleting device proxy )
        // https://github.com/tango-controls/cppTango/issues/540
        //
        QObject *cumbia_custom_widgets_collection = w->get_cumbia_customWidgetCollectionInterface();
        w->show();

        /* register to window manager */
        if(QX11Info::isPlatformX11()) {
            Display *disp = QX11Info::display();
            Window root_win = (Window) w->winId();
            XSetCommand(disp, root_win, argv, argc);
        }

        ret = qu_app->exec();
        delete w;
        delete qu_app;

        if(cumbia_custom_widgets_collection)
            QMetaObject::invokeMethod(cumbia_custom_widgets_collection, "cumbia_tango_free");
    }
    return ret;
}
