#ifndef CUHTTPAUTHMANAGERTOOLS_H
#define CUHTTPAUTHMANAGERTOOLS_H

#include <QString>
#include <QThread>

#ifdef QT_WIDGETS_LIB
    #include <QDialog>
#endif

class CuHttpAuthPrompt_I {
public:
    virtual void getCreds() = 0;
    virtual QObject *qobj() = 0;
};

#ifdef QT_WIDGETS_LIB
class CuHttpCredInputDialog : public QDialog, public CuHttpAuthPrompt_I {
    Q_OBJECT
public:

    CuHttpCredInputDialog(QWidget *parent = nullptr);
    ~CuHttpCredInputDialog();
    void getCreds();
    QObject *qobj();

    void accept();
    void reject();

signals:
    void onCredsReady(const QString& u, const QString& pass);

};
#endif

class CuHttpCredInputCLIPrompt : public QThread, public CuHttpAuthPrompt_I {
 Q_OBJECT
public:
    CuHttpCredInputCLIPrompt(QObject *parent = nullptr);
    ~CuHttpCredInputCLIPrompt();
    void getCreds();
    QObject *qobj();

protected slots:
    void onInputFinished();

protected:
    void run();
signals:
    void onCredsReady(const QString& u, const QString& pass);
private:
    QString m_user, m_pass;
};


class CuHttpAuthInputFactory
{
public:
    CuHttpAuthPrompt_I *create_prompt(bool cli = false, QObject* parent = nullptr) const;
};



#endif // CUHTTPAUTHMANAGERTOOLS_H
