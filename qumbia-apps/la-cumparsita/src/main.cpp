#include <quapplication.h>
#include <cumbiapool.h>
#include <cucontrolsfactorypool.h>
#include "cumparsita.h"

#include <QtDebug>
#include <QMessageBox>

int main(int argc, char *argv[]) {
    // Qt bug: source is set twice because of double setProperty invocation (type QString):
    // https://bugreports.qt.io/browse/QTBUG-48104
    //
    printf(" +------------------------ %s ------------------------+\n\n", argv[0]);
    printf(" + \e[1;31;4mWARNING\e[0m: string properties from UI files are set twice. \e[0;4msource\e[0m included. "
           "This poses performance issues\n");
    printf(" + Watch \e[1;31;4mhttps://bugreports.qt.io/browse/QTBUG-48104\e[0m for resolution\n\n");
    //
    int ret;
    CumbiaPool *p = new CumbiaPool();
    CuControlsFactoryPool fpool;
    QuApplication qu_app( argc, argv, p, &fpool );//
    qu_app.setOrganizationName("elettra");
    qu_app.setApplicationName("cumparsita");
    qu_app.setProperty("author", "Giacomo");
    qu_app.setProperty("mail", "giacomo.strangolino@elettra.eu");
    qu_app.setProperty("phone", "0403758073");
    qu_app.setProperty("office", "T2PT025");

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

        ret = qu_app.exec();
        delete w;

        if(cumbia_custom_widgets_collection)
            QMetaObject::invokeMethod(cumbia_custom_widgets_collection, "cumbia_tango_free");
    }
    return ret;
}
