#ifndef QUEXTRAWIDGETPLUGININTERFACE_H
#define QUEXTRAWIDGETPLUGININTERFACE_H

#include <QWidget>
#include <QString>
#include <cudata.h>
#include <vector>

class Cumbia;
class CumbiaPool;
class CuControlsReaderFactoryI;
class CuControlsWriterFactoryI;
class CuControlsFactoryPool;
class QString;
class QStringList;
class CuContext;

/*!
 * \brief QuXtraWidgetI  defines an interface for widgets created by an implementation of QuXtraWidgetPluginI
 *
 * Since QuXtraWidgetPluginI::create methods instantiate a QWidget, QuXtraWidgetI can be accessed through a
 * *dynamic_cast*.
 *
 * \par Best practise
 * To avoid dynamic casting to QuXtraWidgetI from QWidget, it is a good practise to provide access to the
 * widget useful methods through etither slots or properties.
 * For example, the *source* property could be a shortcut to the *link* and *setLink* methods. For example:
 * \code
 *
   class QuTextView : public QTextEdit, public QuXtraWidgetI, public CuDataListener
   {
    Q_OBJECT
    Q_PROPERTY(QString source READ link WRITE setLink)
    // ...
   }
 * \endcode
 *
 * \par Note
 * Since QuXtraWidgetI subclasses can either be readers, writers or both, the usual *source, target*, *setSource* and *setTarget* methods
 * have been unified under the names *setLink* and *link*.
 * Through the *Qt property* system it is possible to map the *link* methods to either *source* or *target* property names, as aforementioned.
 *
 * \par Real time plot example.
 * Please read the <a href="tutorial_extending_widgets.html">real time plot</a> tutorial for an example.
 *
 * @see QuXtraWidgetPluginLoader
 * @see QuXtraWidgetPluginI
 */
class QuXtraWidgetI {
public:

    enum Type { Reader = 0x1, Writer = 0x2, ReaderWriter = 0x3 };

    virtual ~QuXtraWidgetI() {}

    /*!
     * \brief getContext subclasses must provide a pointer to the CuContext in use
     * \return a pointer to the CuContext used by the subclass.
     */
    virtual CuContext *getContext() const = 0;

    /*!
     * \brief link returns the name of the link (i.e. the source of a reader or the target of a writer)
     *
     * \return a string with the name of the source or target of data
     *
     * This is the equivalent of either source or target and must be implemented by subclasses.
     *
     * Subclasses are advised to provide either a *source* or *target* *property* mapped to
     * *link* and *setLink* to read and write the property, respectively:
     *
     * \codeline Q_PROPERTY(QString source READ link WRITE setLink)
     */
    virtual QString link() const = 0;

    /*!
     * \brief setLink sets the name of the link (source or target)
     *
     * \param s the name of the link to connect to the desired engine.
     *
     * This is the equivalent of either setSource or setTarget and must be implemented by subclasses.
     *
     * Subclasses are advised to provide either a *source* or *target* *property* mapped to
     * *link* and *setLink* to read and write the property, respectively.
     *
     * \codeline Q_PROPERTY(QString source READ link WRITE setLink)
     */
    virtual void setLink(const QString& s) = 0;

    /*!
     * \brief unsetLink remove the source  [target] linked to the reader [writer]
     *
     * Must be implemented by subclasses.
     */
    virtual void unsetLink() = 0;

    /*!
     * \brief getType returns a combination of QuXtraWidgetI::Type flags
     *
     */
    virtual Type getType() const = 0;
};

/*!
 * \brief QuXtraWidgetPluginI provides and interface that subclasses will use to provide one or more widgets
 *        able to display or write values as any other *cumbia-qtcontrols* widget. Even though not strictly
 *        imposed by design, the returned widgets should implement the QuXtraWidgetI interface.
 *
 * Subclasses will implement the *create* methods that are more appropriate.
 * The QWidget instances returned by the *create* methods should implement the QuXtraWidgetI interface, notwithstanding the
 * code design does not force the client to do that.
 * If the objects instantiated by the plugin implement QuXtraWidgetI, they can be *dynamic_cast*-ed at runtime and
 * the methods of the interface can be used.
 * If you choose not to inherit from QuXtraWidgetI, you must provide access to the classes through *Qt properties*.
 *
 * A reader will typically implement both the Cumbia/CuControlsReaderFactoryI and the CumbiaPool/CuControlsFactoryPool versions
 * of the *create* method, as suggested below.
 *
 * Users of the *extra widget plugins* can easily access the plugin that provides a given widget through the QuXtraWidgetPluginLoader
 * helper class. QuXtraWidgetPluginLoader searches amongst the installed *extra widget plugins* by *class name*.
 *
 * The *extra widget plugins* file names must match a given pattern described in the introduction to QuXtraWidgetPluginLoader.
 *
 * The key methods of QuXtraWidgetPluginI are the create functions, defined in several flavours. Their default behavior
 * is returning a null pointer. Subclasses will return valid QWidget intances only from the *create* method they actually reimplement.
 *
 * \par Reader
 * A reader will typically overwrite
 * \li QuXtraWidgetPluginI::create(const QString& name, QWidget *parent, Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac)
 * \li QuXtraWidgetPluginI::create(const QString& name, QWidget *parent, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool)
 *
 * \par Writer
 * A writer will provide valid QWidget instances from:
 * \li QuXtraWidgetPluginI::create(const QString& name, QWidget *parent, Cumbia *cumbia, const CuControlsWriterFactoryI &w_fac)
 * \li QuXtraWidgetPluginI::create(const QString& name, QWidget *parent, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool)
 *
 * \par Reader and writer
 * A reader writer will reimplement
 * \li create(const QString& name, QWidget *parent, Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac, const CuControlsWriterFactoryI &w_fac)
 * \li  QuXtraWidgetPluginI::create(const QString& name, QWidget *parent, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool)
 *
 * @see QuXtraWidgetPluginLoader
 * @see QuXtraWidgetI
 *
 * \par Real time plot example.
 * Please read the <a href="tutorial_extending_widgets.html">real time plot</a> tutorial for an example.
 */
class QuXtraWidgetPluginI
{
public:
    virtual ~QuXtraWidgetPluginI() {}

    /*!
     * \brief create allocate and return a new QWidget implementing QuXtraWidgetI
     * \param name the widget name. If a plugin provides a set of widgets, this parameter can be used to pick a specific one.
     * \param parent the parent widget
     * \param cumbia a pointer to a valid Cumbia object
     * \param r_fac const reference to a CuControlsReaderFactoryI
     *
     *
     * \note By default, returns a nullptr. Subclasses will implement the appropriate create method.
     *
     * \return a new QWidget implementing QuXtraWidgetI
     */
    virtual QWidget *create(const QString& name, QWidget *parent, Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac) {
        Q_UNUSED(name)
        Q_UNUSED(parent)
        Q_UNUSED(cumbia)
        Q_UNUSED(r_fac)
        return nullptr;
    }

    /*!
     * \brief create allocate and return a new QWidget implementing QuXtraWidgetI (writer version)
     * \param name the widget name. If a plugin provides a set of widgets, this parameter can be used to pick a specific one.
     * \param parent the parent widget
     * \param cumbia a pointer to a valid Cumbia object
     * \param w_fac const reference to a CuControlsWriterFactoryI
     *
     * \return a new QWidget implementing QuXtraWidgetI (a writer)
     *
     * \note By default, returns a nullptr. Subclasses will implement the appropriate create method.
     */
    virtual QWidget *create(const QString& name, QWidget *parent, Cumbia *cumbia, const CuControlsWriterFactoryI &w_fac) {
        Q_UNUSED(name)
        Q_UNUSED(parent)
        Q_UNUSED(cumbia)
        Q_UNUSED(w_fac)
        return nullptr;
    }

    /*!
     * \brief create allocate and return a new QWidget implementing QuXtraWidgetI. CumbiaPool constructor version.
     *
     * \param name the widget name. If a plugin provides a set of widgets, this parameter can be used to pick a specific one.
     * \param parent the parent widget
     * \param cumbia_pool a pointer to a valid CumbiaPool object
     * \param fpool const reference to a CuControlsFactoryPool
     *
     * \return a new QWidget implementing QuXtraWidgetI
     *
     * This method creates a widget using the multi engine constructor version.
     *
     * \note By default, returns a nullptr. Subclasses will implement the appropriate create method.
     */
    virtual QWidget *create(const QString& name, QWidget *parent, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool) {
        Q_UNUSED(name)
        Q_UNUSED(parent)
        Q_UNUSED(cumbia_pool)
        Q_UNUSED(fpool)
        return nullptr;
    }

    /*!
     * \brief create allocate and return a new QWidget implementing QuXtraWidgetI. Reader/Writer version.
     *
     * \param name the widget name. If a plugin provides a set of widgets, this parameter can be used to pick a specific one.
     * \param parent the parent widget
     * \param cumbia a pointer to a valid Cumbia object
     * \param r_fac const reference to a CuControlsReaderFactoryI
     * \param w_fac const reference to a CuControlsWriterFactoryI
     *
     * \return a new QWidget implementing QuXtraWidgetI
     *
     * This method creates a widget using the multi engine constructor version.
     * \note writers will return a *nullptr*.
     */
    virtual QWidget *create(const QString& name, QWidget *parent, Cumbia *cumbia,
                            const CuControlsReaderFactoryI &r_fac, const CuControlsWriterFactoryI &w_fac)  {
        Q_UNUSED(name)
        Q_UNUSED(parent)
        Q_UNUSED(cumbia)
        Q_UNUSED(r_fac)
        Q_UNUSED(w_fac)
        return nullptr;
    }
    /*!
     * \brief name returns the name of the plugin
     *
     * \return  the name of the plugin
     */
    virtual QString name() const = 0;

    /*! \brief provides a brief description of the plugin and the classes it provides
     *
     * @return a string with a short description of the plugin.
     *
     * Must be implemented by subclasses
     */
    virtual QString description() const = 0;

    /*!
     * \brief widgetList provides the list of the names of the widgets offered by the plugin
     *
     * \return a QStringList with the names of the widgets made available by the plugin
     */
    virtual QStringList catalogue() const = 0;
};

#define QuXtraWidgetPluginI_iid "eu.elettra.qutils.QuXtraWidgetPluginI"

Q_DECLARE_INTERFACE(QuXtraWidgetPluginI, QuXtraWidgetPluginI_iid)

#endif // QUACTIONEXTENSIONPLUGININTERFACE_H
