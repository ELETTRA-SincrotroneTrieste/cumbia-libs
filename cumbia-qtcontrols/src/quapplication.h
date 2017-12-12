#ifndef QUAPPLICATION_H
#define QUAPPLICATION_H

#include <QApplication>

class QuApplicationPrivate;

class QuAppDBusInterface
{
  public:
    virtual ~QuAppDBusInterface() {}

    virtual void registerApp(const QString& key) = 0;

    virtual void unregisterApp(const QString& key) = 0;
};

#define QuAppDBusInterface_iid "eu.elettra.cumbia-qtcontrols.QuAppDBusInterface"

Q_DECLARE_INTERFACE(QuAppDBusInterface, QuAppDBusInterface_iid)



class QuApplication : public QApplication
{
public:
    QuApplication(int & argc, char **argv);

    void init();

private:
    bool m_loadPlugin();

    QuApplicationPrivate *d;
};

#endif // QUAPPLICATION_H
