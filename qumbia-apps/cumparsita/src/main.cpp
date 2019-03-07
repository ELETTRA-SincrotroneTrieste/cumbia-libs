#include <quapplication.h>
#include "cumparsita.h"

#include <X11/Xlib.h>
#include <QX11Info>
#include <QMessageBox>

#define CVSVERSION "$Name:  $"


int main(int argc, char *argv[])
{
    printf("\e[1;33mmain Cuparsita entering....\e[0m\n\n");
    int ret;
    QuApplication *qu_app = new QuApplication( argc, argv );
    printf("\e[1;33mmain Cuparsita 1....\e[0m\n\n");
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

        printf("\e[1;33mmain Cuparsita 4....\e[0m\n\n");
        Cumparsita *w = new Cumparsita(nullptr);
        printf("\e[1;33mmain Cuparsita 5....\e[0m\n\n");
        w->show();

        /* register to window manager */
        Display *disp = QX11Info::display();
        Window root_win = (Window) w->winId();
        XSetCommand(disp, root_win, argv, argc);

        ret = qu_app->exec();

        printf("RETURNING FROM qu_app->exec(): deleting Cumparsita\n");

        delete w;

        printf("cumparsita deleted\n");

        printf("deleting qu_app\n");
        delete qu_app;
        printf("deleteth qu_app\n");
    }
    return ret;
}
