#ifndef CUAPPLICATIONLAUNCHEREXTENSION_H
#define CUAPPLICATIONLAUNCHEREXTENSION_H

#include <QStringList>
#include <quaction-extension-plugininterface.h>

class QObject;
class CuApplicationLauncherPrivate;

/*! \brief extension to launch applications on demand
 *
 * With this extension, you can try to bring to foreground QuApplications already running or start
 * new ones.
 *
 *
 * \par Example
 *
 * \code
    const char* extensions_plugin_name = "libactions-extension-plugin.so";
    CuPluginLoader pl;
    QString pluginFilePath = pl.getPluginAbsoluteFilePath(QString(), extensions_plugin_name);
    if(!pluginFilePath.isEmpty()) {
        QPluginLoader pluginLoader(pluginFilePath);
        QObject *plugin = pluginLoader.instance();
        if(plugin) {
            QuActionExtensionPluginInterface *action_ex = qobject_cast<QuActionExtensionPluginInterface *>(plugin);
            if(action_ex) {
                QuActionExtensionFactoryI *ae_fac = action_ex->getExtensionFactory();
                QuActionExtensionI *ale = ae_fac->create("CuApplicationLauncherExtension", nullptr);
                CuData cmdline("command", "xclock -analog -twentyfour");
                ale->execute(cmdline);
            }
        }
    }
    \endcode
 *
 */
class CuApplicationLauncherExtension : public QuActionExtensionI
{
public:
    CuApplicationLauncherExtension(const CuContext *ctx = nullptr);
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
