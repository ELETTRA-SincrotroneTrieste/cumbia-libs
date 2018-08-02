#include <QtCore/QCoreApplication>
#include <QtCore/QCommandLineParser>
#include <QtCore/QCommandLineOption>
#include "wsclient.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    printf("app argumetns saiz %d\n", qApp->arguments().size());
    if(qApp->arguments().size() < 2) {
        printf("\e[1;31m USAGE\e[0m: %s source_url1 source_url2 ..., "
               "\n        e.g. %s https://pwma-dev.elettra.eu:10443/v1/cs/tango:"
               "//ken.elettra.trieste.it:20000/test/device/1/float_spectrum_ro\n\n",
               qApp->arguments().first().toStdString().c_str(), qApp->arguments().first().toStdString().c_str());
        return EXIT_FAILURE;
    }
    else {
        QStringList pars = qApp->arguments();
        pars.removeFirst();
        WsClient client(QUrl(QStringLiteral("ws://pwma-dev.elettra.eu:10080/updates")), pars);
        QObject::connect(&client, &WsClient::closed, &a, &QCoreApplication::quit);
        return a.exec();
    }

}
