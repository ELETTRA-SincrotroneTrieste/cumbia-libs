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
    virtual bool inExecution() const = 0;
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
    bool inExecution() const;

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
    bool inExecution() const;

protected slots:
    void onInputFinished();

protected:
    void run();
signals:
    void onCredsReady(const QString& u, const QString& pass);
private:
    QString m_user, m_pass;
};

class CuHttpAuthInputFactoryP;

class CuHttpAuthInputFactory : public QObject
{
    Q_OBJECT
public:
    CuHttpAuthInputFactory();
    virtual ~CuHttpAuthInputFactory();
    CuHttpAuthPrompt_I *create_prompt(bool cli = false, QObject* parent = nullptr) const;
    bool inExecution() const;

public slots:
    void onPromptDestroyed();
private:
    CuHttpAuthInputFactoryP *d;
};



#endif // CUHTTPAUTHMANAGERTOOLS_H
