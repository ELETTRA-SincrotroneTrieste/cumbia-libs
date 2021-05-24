#include "create-delete.h"
#include <quapplication.h>
#include <cumbiapool.h>
#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>
#include <X11/Xlib.h>

int main(int argc, char *argv[])
{
    QuApplication a( argc, argv );
    a.setApplicationName("CreateDelete");
    CumbiaPool *cu_poo = new CumbiaPool();

    CreateDelete *w = new CreateDelete(cu_poo, NULL);

    w->show();

    // exec application loop
        int ret = a.exec();
        // delete resources and return
        delete w;

        for(std::string n : cu_poo->names())
            if(cu_poo->get(n))
                delete cu_poo->get(n);

        return ret;
}
