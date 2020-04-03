#include "qth_widget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QthWidget w;
    w.show();
    return a.exec();
}
