#include "cumparsita.h"
#include <cumbiapool.h>
#include <cumbiaepics.h>
#include <cumbiatango.h>
#include <cuepcontrolsreader.h>
#include <cuepcontrolswriter.h>
#include <cutcontrolsreader.h>
#include <cutcontrolswriter.h>
#include <cutango-world.h>
#include <cuepics-world.h>
#include <cuthreadfactoryimpl.h>
#include <cuserviceprovider.h>
#include <qthreadseventbridgefactory.h>
#include <cumacros.h>

#include <cumbiapool.h>
#include <cuformulaplugininterface.h>
#include <cupluginloader.h>
#include <QPluginLoader>
#include <QUiLoader>
#include <QFile>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QApplication>
#include <QtDebug>

#include "cumuiloader.h"

Cumparsita::Cumparsita(QWidget *parent) :
    QWidget(parent)
{
    QUiLoader *cumLoader= new QUiLoader(this);
    QString ui_file = qApp->arguments().at(1);
    QFile file(ui_file);
    if(file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QWidget *widget = cumLoader->load(&file, this);
        file.close();
        QVBoxLayout * vblo = new QVBoxLayout;
        vblo->addWidget(widget);
        qDebug() << __PRETTY_FUNCTION__ << this << widget << widget->children().size();
        setLayout(vblo);
        setWindowTitle(widget->windowTitle() + QString(" - %1").arg(qApp->applicationName()));
    }
    else {
        QMessageBox::critical(this, "Error opening file", "Error opening file " + ui_file + " in read mode:\n" +
                              file.errorString());
    }
}

Cumparsita::~Cumparsita()
{
    printf("\e[1;31m~Cumparsita\e[0m\n");
    delete findChild<QUiLoader *>();
    printf("deleted cum loader\n");
}
