#include "cupluginloader.h"
#include <QProcessEnvironment>
#include <QDir>
#include <QPluginLoader>

CuPluginLoader::CuPluginLoader()
{

}

/*! \brief get the absolute file path of the plugin with name *name*.
 *
 * @return the absolute file path of the plugin with the given name, an empty string otherwise
 *
 * @param default_plugin_path the default cumbia plugin installation directory (e.g. CUMBIA_QTCONTROLS_PLUGIN_DIR
 *        for cumbia-qtcontrols projects or QUMBIA_PLUGIN_DIR for plugins that include qumbia-plugins.pri from
 *        the qumbia-plugins moudule)
 *
 * @param name the exact name of the desired plugin, for example "libactions-extension-plugin.so" or
 *        "libcumbia-dbus-plugin.so"
 *
 * \note Plugin search gives priority to folders listed in the  <strong>CUMBIA_PLUGIN_PATH</strong> environment
 *       variable. *default_plugin_path* folder is searched last.
 *
 * @see getPluginAbsoluteFilePaths
 */
QString CuPluginLoader::getPluginAbsoluteFilePath(const QString& default_plugin_path, const QString& name) {
    QStringList list;
    QRegExp re(name);
    list = getPluginAbsoluteFilePaths(default_plugin_path, re);
    if(list.size() > 0)
        return list.first();
    return QString();
}

/**
 * @brief CuPluginLoader::getPluginPath returns the plugin path from the CUMBIA_PLUGIN_PATH environment variable
 * @return the plugin path set with the environment variable CUMBIA_PLUGIN_PATH or an empty string if the
 * variable is unset
 */
QString CuPluginLoader::getPluginPath() const {
    return QString(getenv("CUMBIA_PLUGIN_PATH"));
}

/*! \brief get a list of the absolute file paths of the plugins which name matches the regular expression *match*.
 *
 * @return a list of the absolute file paths of the plugins matching the given *match*, or an empty list if no plugin
 *         file name matches the given pattern
 *
 * @param default_plugin_path the default cumbia plugin installation directory (e.g. CUMBIA_QTCONTROLS_PLUGIN_DIR
 *        for cumbia-qtcontrols projects or QUMBIA_PLUGIN_DIR for plugins that include qumbia-plugins.pri from
 *        the qumbia-plugins moudule)
 *
 * @param match a regular expression to match the desired plugin name. For example the ".*context-menu-actions\.so"
 *        can be used to find all the plugins extending the contextual menu of a cumbia-qtcontrols widget.
 *
 * \note Plugin search gives priority to folders listed in the  <strong>CUMBIA_PLUGIN_PATH</strong> environment
 *       variable. *default_plugin_path* folder is searched last.
 *
 * @see getPluginAbsoluteFilePath
 */
QStringList CuPluginLoader::getPluginAbsoluteFilePaths(const QString& default_plugin_path, const QRegExp &match)
{
    QStringList sl;
    QProcessEnvironment pe = QProcessEnvironment::systemEnvironment();
    QString custom_plugin_dir = pe.value("CUMBIA_PLUGIN_PATH", "");
    QStringList plugin_dirs = custom_plugin_dir.split(":", QString::SkipEmptyParts);
    QStringList plugin_names; // do not return duplicate plugins (with the same file name)
    plugin_dirs.append(default_plugin_path); // last
    foreach(QString plugindir, plugin_dirs) {
        QDir pluginsDir(plugindir);
        QStringList entryList = pluginsDir.entryList(QDir::Files);
        for(int i = 0; i < entryList.size(); i++) {
            QString fileName = entryList[i];
            if(fileName.contains(match) && !plugin_names.contains(fileName)) {
                sl << pluginsDir.absoluteFilePath(fileName);
                plugin_names << fileName;
            }
        }
    }
    return sl;
}
