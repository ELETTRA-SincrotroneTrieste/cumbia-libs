#include <QCoreApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include "cutelegramctrl.h"
#include "dbctrl.h"
#include <QtDebug>

#include "../cumbia-telegram-defs.h" // LOCALSERVER_NAME

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription("control the cumbia-telegram bot");
    parser.addHelpOption();
    parser.addVersionOption();

    // auth
    QCommandLineOption users(QStringList() << "u" << "show-users", "show the list of users (authorized and not)");
    QCommandLineOption addUser(QStringList() << "a" << "authorize", "authorize the user with the given id", "UID");
    QCommandLineOption remUser(QStringList() << "r" << "revoke", "revoke authorization for the given user_id", "UID");
    QCommandLineOption dbfile(QStringList() << "f" << "dbfile", "SQLite database file used by the cumbia-telegram bot daemon",
                              "/tmp/botdb.dat");
    QCommandLineOption stats(QStringList() << "s" << "stats", "Display server load statistics");

    parser.addOption(remUser);
    parser.addOption(users);
    parser.addOption(dbfile);
    parser.addOption(addUser);
    parser.addOption(stats);

    parser.addPositionalArgument("user_id", "the user id");

    parser.process(a);

    const QStringList args = parser.positionalArguments();

    CuTelegramCtrl tc;
    QString dbf = parser.value(dbfile);
    if(dbf.isEmpty()) {
        tc.print_error("main", "option \"-f\" (or \"--dbfile\" sqlite_filename is mandatory");
        return EXIT_FAILURE;
    }
    DbCtrl *dbctrl = new DbCtrl(dbf);
    bool ok = tc.init(QString(LOCALSERVER_NAME), dbctrl);
    if(ok) {
        QString uid = parser.value(addUser);
        int u_id = uid.toInt(&ok);
        if(ok && u_id > 0) {
            tc.authorize(uid.toInt(), true);
        }
        else {
            uid = parser.value(remUser);
            u_id = uid.toInt(&ok);
            if(ok && u_id > 0) {
                tc.authorize(uid.toInt(), false);
            }
        }

        if(parser.isSet(users)) {
            tc.printUsers();
        }
        if(parser.isSet(stats))
            ok = tc.requestStats();

        if(!ok) {
            tc.print_error();
        }
    }
    else {
        tc.print_error("main", "failed to initialize cumbia-telegram-control");
    }

}
