#ifndef CUAPPLICATIONLAUNCHER_H
#define CUAPPLICATIONLAUNCHER_H

#include <QStringList>

class CuApplicationLauncherPrivate;

class CuApplicationLauncher
{
public:
    CuApplicationLauncher(const QStringList& args);

    void start();

private:
    CuApplicationLauncherPrivate *d;
    void m_loadCumbiaPlugin();
};

#endif // CUAPPLICATIONLAUNCHER_H
