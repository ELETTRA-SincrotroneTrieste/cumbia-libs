#include "cumuiloader.h"
#include <QtDebug>
#include <QPluginLoader>

CumUiLoader::CumUiLoader(QObject *parent) : QUiLoader (parent)
{
    qDebug() << __PRETTY_FUNCTION__ << availableWidgets();
}

CumUiLoader::~CumUiLoader()
{
    qDebug() << __PRETTY_FUNCTION__ << "deleting CumUILoader";
}


QWidget *CumUiLoader::createWidget(const QString &className, QWidget *parent, const QString &name)
{
    qDebug() << __PRETTY_FUNCTION__ << className << parent << name;
    return  QUiLoader::createWidget(className, parent, name);
}
