#ifndef CUPLUGINLOADER_H
#define CUPLUGINLOADER_H

#include <QStringList>
#include <QRegularExpression>
#include <QPluginLoader>
#include <QObject>

/*! \brief Helper class that loads Qt plugins searching in the default cumbia plugin path and in directories
 *         listed in a proper environment variable.
 *
 * Plugins can be loaded from a default cumbia plugin library directory which is defined in the cumbia-qtcontrols.pri
 * file and passed to the compiler with a -D option. The name is:
 *
 * \li CUMBIA_QTCONTROLS_PLUGIN_DIR
 *
 *
 * Plugins are searched also in one of the *colon separated list* of directories exported through the
 * *CUMBIA_PLUGIN_PATH* environment variable.
 *
 */
class CuPluginLoader
{
public:
    CuPluginLoader();

    QStringList getPluginAbsoluteFilePaths(const QString &default_plugin_path, const QRegularExpression& match= QRegularExpression(".*"));

    QString getPluginAbsoluteFilePath(const QString &default_plugin_path, const QString &name);
    QString getPluginPath() const;
    QString getDefaultPluginPath() const;

    template <class T>
    T* get(const QString& name, QObject **plugin_as_qobject, const QString& default_plugin_path = QString()) {
        QObject *plugin;
        if(plugin_as_qobject != nullptr)
            *plugin_as_qobject = nullptr;
        T* pl = nullptr;
        QString plugin_path = getPluginAbsoluteFilePath(default_plugin_path, name);
        QPluginLoader loader(plugin_path);
        plugin = loader.instance();
        if(plugin) {
            pl = qobject_cast<T *>(plugin);
            if(!pl)
                printf("\e[1;31m*\e[0m CuPluginLoader.get: failed to cast the plugin provided by \"%s\" into the desired type\n",
                     name.toStdString().c_str());
        }
        else
            printf("\e[1;31m*\e[0m CuPluginLoader.get: failed to load plugin \"%s\" from \"%s\": \"%s\"\n",
                   name.toStdString().c_str(), plugin_path.toStdString().c_str(), loader.errorString().toStdString().c_str());
        if(plugin_as_qobject != nullptr)
            *plugin_as_qobject = plugin;
        return pl;
    }
};

#endif // CUPLUGINLOADER_H
