#ifndef CUPLUGINLOADER_H
#define CUPLUGINLOADER_H

#include <QStringList>
#include <QRegExp>

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

    QStringList getPluginAbsoluteFilePaths(const QString &default_plugin_path, const QRegExp& match= QRegExp(".*"));

    QString getPluginAbsoluteFilePath(const QString &default_plugin_path, const QString &name);
    QString getPluginPath() const;
    QString getDefaultPluginPath() const;
};

#endif // CUPLUGINLOADER_H
