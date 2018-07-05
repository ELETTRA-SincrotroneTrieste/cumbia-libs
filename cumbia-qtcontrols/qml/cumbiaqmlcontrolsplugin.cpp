#include "cumbiaqmlcontrolsplugin.h"

#include <cumbiapool_o.h>
#include "qmlreaderbackend.h"
#include "qmlchartbackend.h"
#include <qqml.h>
#include <QtDebug>

void CumbiaQmlControlsPlugin::registerTypes(const char *uri)
{
    const QString prefix = baseUrl().toString();
    qDebug() << "pfefix is " << prefix;
    qmlRegisterType(QUrl(prefix + "/CumbiaCircularGauge.qml"), uri, 1, 0, "CumbiaCircularGauge");
    qmlRegisterType(QUrl(prefix + "/CumbiaLabel.qml"), uri, 1, 0, "CumbiaLabel");
    qmlRegisterType(QUrl(prefix + "/CumbiaTrendPlot.qml"), uri, 1, 0, "CumbiaTrendPlot");

    qmlRegisterType<CumbiaPool_O>(uri, 1, 0, "CumbiaPool_O");
    qmlRegisterType<QmlReaderBackend>(uri, 1, 0, "QmlReaderBackend");
    qmlRegisterType<QmlChartBackend>(uri, 1, 0, "QmlChartBackend");
}
