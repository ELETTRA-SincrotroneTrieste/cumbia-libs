#ifndef CUAPPLICATIONLAUNCHEREXTENSION_H
#define CUAPPLICATIONLAUNCHEREXTENSION_H

#include <QStringList>
#include <quaction-extension-plugininterface.h>

class QObject;
class CuApplicationLauncherPrivate;

class CuApplicationLauncherExtension : public QuActionExtensionI
{
public:
    CuApplicationLauncherExtension(const CuContext *ctx);
    ~CuApplicationLauncherExtension();

    void start();

private:
    CuApplicationLauncherPrivate *d;
    void m_loadCumbiaPlugin();

    // QuActionExtensionI interface
public:
    QString getName() const;
    CuData execute(const CuData &in);
    std::vector<CuData> execute(const std::vector<CuData>& );
    QObject *get_qobject();
    const CuContext *getContext() const;
    std::string message() const;
    bool error() const;
};

#endif // CUAPPLICATIONLAUNCHEREXTENSION_H
