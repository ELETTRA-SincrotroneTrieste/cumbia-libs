#include "quxtrawidgetpluginloader.h"
#include "cupluginloader.h"
#include <QPluginLoader>

class QuXtraWidgetPluginLoaderPrivate {
public:

};

QuXtraWidgetPluginLoader::QuXtraWidgetPluginLoader()
{
    d = new QuXtraWidgetPluginLoaderPrivate;
}

QuXtraWidgetPluginLoader::~QuXtraWidgetPluginLoader()
{
    delete d;
}

/*!
 * \brief QuXtraWidgetPluginLoader::isAvailable returns true if the widget with the given class name is made provided by one
 *        of the available *extrawidget* plugins.
 *
 * \param classnam the widget name (widget class name)
 *
 * \return true if the widget with the given name is available, false otherwise
 */
bool QuXtraWidgetPluginLoader::isAvailable(const QString &classnam)
{
    return getPlugin(classnam) != nullptr;
}


QuXtraWidgetPluginI *QuXtraWidgetPluginLoader::getPlugin(const QString &classnam) const
{
    CuPluginLoader plo;
    const char* xw_pattern = "(?:e){0,1}x(?:tra){0,1}widget[-]{0,1}plugin.*\\.so";
    QStringList plpaths = plo.getPluginAbsoluteFilePaths(QString(), QRegularExpression(xw_pattern));
    foreach(QString pluginFilePath, plpaths) {
        printf("trying to load plugin %s\n",  qstoc(pluginFilePath));
        QPluginLoader pluginLoader(pluginFilePath);
        QObject *plugin = pluginLoader.instance();
        if(plugin) {
            QuXtraWidgetPluginI * xwi = qobject_cast<QuXtraWidgetPluginI *>(plugin);
            if(!xwi)
                perr("QuXtraWidgetPluginLoader.getPlugin: plugin \"%s\" is not a QuXtraWidgetPluginI implementation\n", qstoc(pluginFilePath));
            else if(xwi->catalogue().contains(classnam)) {
                return xwi;
            }
        }
        else {
            perr("QuXtraWidgetPluginLoader.getPlugin:  QPluginLoader failed to load \"%s\": %s", qstoc(pluginFilePath), qstoc(pluginLoader.errorString()));
        }
    }
    perr("QuXtraWidgetPluginLoader.getPlugin: no plugin is providing class \"%s\"", qstoc(classnam));
    return nullptr;
}
