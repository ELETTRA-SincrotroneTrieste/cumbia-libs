#include "cutelegramctrl.h"
#include "dbctrl.h"
#include <QLocalSocket>
#include <QMap>
#include <QList>
#include <QtDebug>

class CuTelegramCtrlPrivate {
public:
    QLocalSocket *so;
    bool error;
    QString msg;
    DbCtrl *db;
};

CuTelegramCtrl::CuTelegramCtrl(QObject *parent) : QObject(parent)
{
    d = new CuTelegramCtrlPrivate;
    d->so = new QLocalSocket(this);
    connect(d->so, SIGNAL(error(QLocalSocket::LocalSocketError)), this,
            SLOT(onLocalSocketError(QLocalSocket::LocalSocketError)));
    d->error = false;
    d->db = nullptr;
}

bool CuTelegramCtrl::init(const QString &servernam, DbCtrl *db)
{
    d->db = db;
    d->so->connectToServer(servernam);
    d->error = !d->so->waitForConnected();
    if(d->error) {
        d->msg = QString("error opening local socket in RW mode (name %1): %2")
                .arg(servernam).arg(d->so->errorString());
        print_error("CuTelegramCtrl.init", d->msg);
    }
    return !d->error;
}

CuTelegramCtrl::~CuTelegramCtrl()
{
    if(d->so && d->so->isOpen())
        d->so->close();
    delete d;
}

bool CuTelegramCtrl::error() const
{
    return d->error;
}

bool CuTelegramCtrl::printUsers()
{
    QList<QMap <QString, QString> > umap;
    if(d->db) {
        QString s1, s2;
        QStringList keys = QStringList() << "id" << "uname" << "first_name" << "last_name" << "join_date";
        d->db->getUserInfo(umap);
        for(int i = 0; i < umap.size(); i++) {
            const QMap<QString, QString> &ma = umap[i];
            int auth = ma["authorized"].toInt();
            if(auth == 0) {
                s1 = "\e[1;31mX\e[0m";
                s2 = "\e[1;31;4mUNAUTHORIZED\e[0m";
            }
            else if(auth > 0) {
                s1 = "\e[1;32mOK\e[0m";
                s2 = "\e[1;32mAUTHORIZED\e[0m";
            }
            else {
                s1 = "\e[1;35m?\e[0m";
                s2 = "\e[0;35;4mAUTH PENDING\e[0m";
            }
            printf("%s\t", s1.toUtf8().data());
            foreach(const QString& s, keys) {
                printf("%s\t",  ma[s].toUtf8().data());
            }
            printf("%s\t\n", s2.toUtf8().data());
        }
    }
    return !d->db->error();
}

QString CuTelegramCtrl::message() const
{
    return d->msg;
}
void CuTelegramCtrl::authorize(int uid, bool authorize)
{
    if(!d->db)
        return;
    print(QString("setting authorization to %1 for user id %2").arg(authorize).arg(uid));
    bool success = d->db->setAuthorized(uid, authorize);
    print_success(success);
    if(!success)
        print_error("DbCtrl", d->db->message());
    else if(d->db->hasMessage())
        printf("\e[1;32m-->\e[0m %s\n", d->db->message().toUtf8().data());
    send("auth_changed");
}

bool CuTelegramCtrl::send(const QString &s)
{
    qint64 bytes_written = d->so->write(s.toUtf8());
    d->error = bytes_written < 0;
    if(!d->error)
        d->error = !d->so->waitForBytesWritten();
    if(d->error) {
        d->msg = d->so->errorString();
        print_error("CuTelegramCtrl.send", d->msg);
    }
    return !d->error;
}

void CuTelegramCtrl::onNewData()
{
    QString s = QString(d->so->readAll());
    qDebug() << __PRETTY_FUNCTION__ << "received message: " << s;
}

void CuTelegramCtrl::onLocalSocketError(QLocalSocket::LocalSocketError e)
{
    Q_UNUSED(e);
    d->error = true;
    d->msg = qobject_cast<QLocalSocket *>(sender())->errorString();
}

void CuTelegramCtrl::print_error(const QString &origin, const QString &msg)
{
    fprintf(stderr, "\e[1;31;4merror\e[0m: %s: \e[1m%s\e[0m\n", origin.toUtf8().data(), msg.toUtf8().data());
}

void CuTelegramCtrl::print(const QString &s)
{
    printf("%s...", s.toUtf8().data());
    fflush(stdout);
}

void CuTelegramCtrl::print_success(bool ok)
{
    ok? printf("\t\t[\e[1;32mOK\e[0m]\n") : printf("\t\t[\e[1;31mFAILED\e[0m]\n");
}

