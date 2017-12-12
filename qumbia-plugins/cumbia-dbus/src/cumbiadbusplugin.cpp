#include "cumbiadbusplugin.h"
#include <cumacros.h>
#include <QtDebug>

CumbiaDBusPlugin::CumbiaDBusPlugin(QObject *parent) : QObject(parent)
{
    qDebug() << __FUNCTION__ << "constructor: parent" << parent;
}

void CumbiaDBusPlugin::registerApp(const QString &key)
{
    qDebug() << __FUNCTION__ << "key" << key;
}

void CumbiaDBusPlugin::unregisterApp(const QString &key)
{
    qDebug() << __FUNCTION__  << "key" << key;
}
