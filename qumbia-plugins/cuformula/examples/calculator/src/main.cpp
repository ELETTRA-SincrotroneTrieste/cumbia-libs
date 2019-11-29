#include <quapplication.h>
#include "calculator.h"

#include <cumacros.h>
#include <cumbiapool.h>
#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>


#define CVSVERSION "$Name:  $"


int main(int argc, char *argv[])
{
    int ret;
    QuApplication qu_app( argc, argv );
    qu_app.setOrganizationName("Elettra");
    qu_app.setApplicationName("cumbiacalc");
    QString version(CVSVERSION);
    qu_app.setApplicationVersion(version);
    qu_app.setProperty("author", "Giacomo");
    qu_app.setProperty("mail", "giacomo.strangolino@elettra.eu");
    qu_app.setProperty("phone", "0403758073");
    qu_app.setProperty("office", "T2PT025");
    qu_app.setProperty("hwReferent", "$HW_REFERENT$"); /* name of the referent that provides the device server */
    
    if(qu_app.arguments().count() < 2) {
        printf("\nUsage: %s formula\n\n", qstoc(qu_app.arguments().first()));
        ret = EXIT_SUCCESS;
    }
    else {
        bool verb =  qu_app.arguments().contains("-v") || qu_app.arguments().contains("--verbose");
        CumbiaPool *cu_p = new CumbiaPool();
        Calculator *calc = new Calculator(cu_p, nullptr, verb);
        QObject::connect(calc, SIGNAL(finished()), &qu_app, SLOT(quit()));

        QStringList args(qu_app.arguments());
        calc->read(args.at(1));
        if(calc->hasFinished())
            calc->code() == 0 ? ret = EXIT_SUCCESS : ret = EXIT_FAILURE;
        else
            ret = qu_app.exec();

        delete calc;
        Cumbia *c;
        c = cu_p->get("tango");
        if(c) delete c;
        c = cu_p->get("epics");
        if(c) delete c;
    }
    return 0;
}
