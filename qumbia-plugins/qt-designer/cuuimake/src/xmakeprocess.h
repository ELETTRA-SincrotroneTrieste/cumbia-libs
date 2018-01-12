#ifndef XMAKEPROCESS_H
#define XMAKEPROCESS_H

#include <QString>
#include <QObject>

class XMakeProcess : public QObject
{
    Q_OBJECT
public:
    XMakeProcess();

    bool qmake();

    bool make(const QString &minus_jN);

    bool error() const { return m_error; }

    bool clean();

    QString lastError() const { return m_lastError; }

private slots:
    void onStdErrReady();

    void onStdOutReady();

private:
    bool m_error;

    QString m_lastError;

};

#endif // XMAKEPROCESS_H
