#include "label.h"
#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if(qApp->arguments().count() < 2)
    {
        QMessageBox::information(0, "Usage", argv[0] + QString(" test/device/name/attribute|command"));
        exit(EXIT_FAILURE);
    }
    else
    {
        WidgetWithLabel w;
        w.show();
        return a.exec();
    }

}
