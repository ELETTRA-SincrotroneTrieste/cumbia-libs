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

    void print(const QString& s);

    void print_success(bool ok);

    void authorize(int uid, bool authorize);

public slots:
    bool send(const QString& s);

signals:

private slots:
    void onNewData();

    void onLocalSocketError(QLocalSocket::LocalSocketError e);

private:
    CuTelegramCtrlPrivate *d;
};

#endif // CUTELEGRAMCTRL_H
