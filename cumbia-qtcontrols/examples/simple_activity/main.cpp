#include <QApplication>
#include "simplea.h"

int main(int argc, char **argv)
{
    QApplication a(argc, argv );
    a.setApplicationName("simple_activity");
    SimpleA *w = new SimpleA(0);
    w->show();
    int ret = a.exec();
    delete w;
    return ret;
}
