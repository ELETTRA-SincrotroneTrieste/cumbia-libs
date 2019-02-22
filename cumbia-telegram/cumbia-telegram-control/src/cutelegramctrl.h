#ifndef CUTELEGRAMCTRL_H
#define CUTELEGRAMCTRL_H

#include <QObject>
#include <QLocalSocket>

class CuTelegramCtrlPrivate;
class DbCtrl;

class CuTelegramCtrl : public QObject
{
    Q_OBJECT
public:
    explicit CuTelegramCtrl(QObject *parent = nullptr);

    bool init(const QString& servernam, DbCtrl *db);

    ~CuTelegramCtrl();

    bool error() const;

    bool printUsers();

    QString message() const;

    void print_error(const QString& origin, const QString& msg);

    void print_error();

    void print(const QString& s);

    void print_success(bool ok);

    void authorize(int uid, bool authorize);

    void printStats(const QString& s) const;

    bool requestStats();

    void onNewData(const QByteArray& ba);

public slots:
    bool send(const QString& s, bool wait_for_reply);

signals:

private slots:

    void onLocalSocketError(QLocalSocket::LocalSocketError e);

    void m_printReply(const QString& r);

    QString m_senderHeader(const QJsonObject &o);

private:
    CuTelegramCtrlPrivate *d;
};

#endif // CUTELEGRAMCTRL_H
