#include "erw.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ERW w;
    w.show();

    return a.exec();
}
