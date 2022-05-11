#include "cumparsita.h"
#include <cuserviceprovider.h>
#include <qthreadseventbridgefactory.h>
#include <cumacros.h>

#include <cuformulaplugininterface.h>
#include <cupluginloader.h>
#include <QPluginLoader>
#include <QUiLoader>
#include <QFile>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QApplication>
#include <QtDebug>
#include <QtUiPlugin/QDesignerCustomWidgetCollectionInterface>
#include <QDir>

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
    resize(320, 240);
}

QObject *Cumparsita::get_cumbia_customWidgetCollectionInterface() const
{
    QObject *object = nullptr;
    QStringList pluginPaths = findChild<QUiLoader *>()->pluginPaths();
    for(int i = 0; i < pluginPaths.size(); i++) {
        QString path = pluginPaths[i];
        QDir dir(path);
        QStringList files = dir.entryList(QStringList () << "*.so", QDir::Files|QDir::Executable);
        foreach(QString file, files) {
            QPluginLoader loader(path + "/" + file);
            object = (loader.instance());
            QString nsig = object->metaObject()->normalizedSignature("cumbia_free()");
            if(object && object->metaObject()->indexOfSlot(qstoc(nsig)) > -1)
                return object;
        }
    }
    return nullptr;
}

Cumparsita::~Cumparsita() {
    delete findChild<QUiLoader *>();
}
