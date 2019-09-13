#ifndef QUXTRAWIDGETPLUGINLOADER_H
#define QUXTRAWIDGETPLUGINLOADER_H

#include <QString>
#include <quxtrawidgetplugininterface.h>

class QuXtraWidgetPluginLoaderPrivate;

/*!
 * \brief The QuXtraWidgetPluginLoader class finds the plugin that exports a widget with a given file name
 *
 * The plugin file names providing *extra widgets* (see the QuXtraWidgetPluginI) must be in one of the following forms:
 *
 * \li mysetofwidgets-xwidgetplugin.so
 * \li mysetofwidgets-extrawidgetplugin.so
 * \li mysetofwidgets-xtrawidgetplugin.so
 * \li mysetofwidgets-xwidget-plugin.so
 * \li mysetofwidgets-xtrawidget-plugin.so
 * \li mysetofwidgets-extrawidget-plugin.so
 * \li my-xwidgetplugin-set1.so
 *
 * More generally, the file name must contain the following *regular expression*:
 *
 * \code
 * (?:e){0,1}x(?:tra){0,1}widget[-]{0,1}plugin.*\.so
 * \endcode
 *
 * that matches somewhere in the name one of the keywords *xwidgetplugin, xtrawidgetplugin, extrawidgetplugin* with an optional dash
 * between *widget* and *plugin*.
 *
 */
class QuXtraWidgetPluginLoader
{
public:
    QuXtraWidgetPluginLoader();

    virtual ~QuXtraWidgetPluginLoader();

    bool isAvailable(const QString& classnam);

    QuXtraWidgetPluginI *getPlugin(const QString& classnam) const;

private:
    QuXtraWidgetPluginLoaderPrivate *d;
};

#endif // QUXTRAWIDGETPLUGINLOADER_H
