#include "cumbiaqmlcontrolsplugin.h"

#include "cumbiapoolfactory.h"
#include "qmlreaderbackend.h"
#include <qqml.h>
#include <QtDebug>

void CumbiaQmlControlsPlugin::registerTypes(const char *uri)
{
    const QString prefix = baseUrl().toString();
    qDebug() << "pfefix is " << prefix;
    qmlRegisterType(QUrl(prefix + "/MyRectangle.qml"), uri, 1, 0, "MyRectangle");
    qmlRegisterType(QUrl(prefix + "/CumbiaCircularGauge.qml"), uri, 1, 0, "CumbiaCircularGauge");

    qmlRegisterType<CumbiaPoolFactory>(uri, 1, 0, "CumbiaPoolFactory");
    qmlRegisterType<QmlReaderBackend>(uri, 1, 0, "QmlReaderBackend");
}
