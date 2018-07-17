#include "cumbiaqmlcontrolsplugin.h"

#include <cumbiapool_o.h>
#include "qmlreaderbackend.h"
#include "qmlwriterbackend.h"
#include "qmlchartbackend.h"
#include <qqml.h>
#include <QtDebug>

void CumbiaQmlControlsPlugin::registerTypes(const char *uri)
{
    const QString prefix = baseUrl().toString();
    qmlRegisterType(QUrl(prefix + "/CumbiaCircularGauge.qml"), uri, 1, 0, "CumbiaCircularGauge");
    qmlRegisterType(QUrl(prefix + "/CumbiaLabel.qml"), uri, 1, 0, "CumbiaLabel");
    qmlRegisterType(QUrl(prefix + "/CumbiaTrendChart.qml"), uri, 1, 0, "CumbiaTrendChart");
    qmlRegisterType(QUrl(prefix + "/CumbiaSpectrumChart.qml"), uri, 1, 0, "CumbiaSpectrumChart");

    // writers
    qmlRegisterType(QUrl(prefix + "/CumbiaDial.qml"), uri, 1, 0, "CumbiaDial");
    qmlRegisterType(QUrl(prefix + "/CuButton.qml"), uri, 1, 0, "CuButton");
    qmlRegisterType(QUrl(prefix + "/CuWButton.qml"), uri, 1, 0, "CuWButton");
    qmlRegisterType(QUrl(prefix + "/CuTumbler.qml"), uri, 1, 0, "CuTumbler");

    qmlRegisterType<CumbiaPool_O>(uri, 1, 0, "CumbiaPool_O");
    qmlRegisterType<QmlReaderBackend>(uri, 1, 0, "QmlReaderBackend");
    qmlRegisterType<QmlWriterBackend>(uri, 1, 0, "QmlWriterBackend");
    qmlRegisterType<QmlChartBackend>(uri, 1, 0, "QmlChartBackend");
}
