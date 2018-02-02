#ifndef QUAPPDBUSPLUGININTERFACE_H
#define QUAPPDBUSPLUGININTERFACE_H

#include <QObject>
#include <QList>
#include <QStringList>

class QuApplication;

/*! \brief a class that groups information about a QuApplication
 *
 * \ingroup plugins
 */
class QuAppInfo
{
public:

    QuAppInfo() { pid = -1; }

    /** \brief QuAppInfo constructor that accepts the <em>full</em> command line as provided by
     *         QApplication::arguments, including argv[0].
     *
     * @param _args the value returned by QApplication::arguments, argv[0] included.
     *
     * Initializes the fields of the QuAppInfo structure. This will separate the application
     * name and the command line options provided with the input parameter.
     *
     * \li The exename method returns the argv[0]
     * \li args field (public) contains the command line <em>options</em>, without the argv[0]
     * \li path returns the application path, if provided, that is argv[0] - exename
     * \li argv0 returns the argv[0]
     */
    QuAppInfo(const QStringList& _args, const QString& dbus_servnam = QString()) {
        if(_args.size() > 0) {
            m_argv0 = _args.first();
            args = _args;
            args.removeFirst();
            dbus_servicename = dbus_servnam;
            pid = -1;
        }
    }

    QuAppInfo(const QString& exenam, const QStringList& argums, const QString& dbus_servicenam) {
        m_argv0 = exenam;
        args = argums;
        dbus_servicename = dbus_servicenam;
    }

    /** \brief QuAppInfo is empty if argv[0] is empty
     *
     */
    bool isEmpty() const {
        return m_argv0.isEmpty();
    }

    /** \brief Returns the executable name, that is argv[0] without the path
     *
     */
    QString exename() const {
        if(m_argv0.contains("/"))
            return m_argv0.split("/",QString::SkipEmptyParts).last();
        return m_argv0;
    }

    /** \brief Returns the path of the executable, if provided in the constructor.
     *
     * If the QuApplication has been build without the path, an empty string must be expected.
     */
    QString path() const {
        QString out(m_argv0);
        out.remove(exename());
        return out;
    }

    /** \brief Returns argv[0]
     */
    QString argv0() const {
        return m_argv0;
    }

    QString dbus_servicename;
    QStringList args;
    pid_t pid;

private:
    QString m_argv0;
};

/** \brief an interface defining the behaviour of a *cumbia* application
 *         that wants to register with the DBus session bus.
 *
 * \ingroup plugins
 *
 * QuAppDBus class in qumbia-plugins implements this interface.
 * Plugins can be found in the qumbia-plugins directory of cumbia-libs.
 */
class QuAppDBusInterface
{
public:
    virtual ~QuAppDBusInterface() {}

    virtual void registerApp(QuApplication *app) = 0;

    virtual void unregisterApp(QuApplication *app) = 0;

    virtual QString getServiceName(QuApplication *app) const = 0;
};

/*! \brief interface to listen for applications registering and unregistering
 *         to/from the DBus session bus.
 *
 * \ingroup plugins
 *
 */
class QuAppDBusControllerListener
{
public:
    virtual void onAppRegistered(const QuAppInfo& ai) = 0;

    virtual void onAppUnregistered (const QuAppInfo& ai) = 0;
};

/*! this class remotely controls an application registered with the DBus session bus
 *
 * \ingroup plugins
 *
 * The implementation of this interface can:
 * \li find other applications registered with DBus by name and arguments
 * \li close an application matching the characteristics described in QuAppInfo
 * \li raise an application matching the characteristics described in QuAppInfo
 */
class QuAppDBusControllerInterface
{
public:
    virtual ~QuAppDBusControllerInterface() {}

    virtual QList<QuAppInfo> findApps(const QStringList& args) = 0;

    virtual void close(const QuAppInfo &ai) = 0;

    virtual void raise(const QuAppInfo &ai) = 0;

    virtual void addCtrlListener(QuAppDBusControllerListener *l) = 0;

    virtual void removeCtrlListener(QuAppDBusControllerListener *l) = 0;

    virtual void start_monitor(const QString& dbus_servicenam) = 0;
    virtual void stop_monitor(const QString& dbus_servicenam) = 0;

};

/** \brief Interface for the QuDBus plugin.
 *
 * \ingroup plugins
 *
 * The DBus plugin for QuApplication must implement this interface.
 *
 * CumbiaDBusPlugin implements this interface. Plugins can be found
 * in the qumbia-plugins directory of cumbia-libs.
 */
class QuDBusPluginInterface
{
public:
    virtual ~QuDBusPluginInterface() {}

    /*! returns the QuAppDBusInterface that registers and unregisters
     *  an application with DBus
     *
     * @return an instance of a QuAppDBusInterface implementation.
     */
    virtual QuAppDBusInterface *getAppIface() const = 0;

    /*! \brief returns the QuAppDBusControllerInterface interface
     *
     * @return QuAppDBusControllerInterface the object that controls an application
     *         raising or closing it or finds applications registered with DBus
     */
    virtual QuAppDBusControllerInterface *getAppCtrlIface() const = 0;
};

#define QuDBusPluginInterface_iid "eu.elettra.qudbus.QuDBusPluginInterface"

Q_DECLARE_INTERFACE(QuDBusPluginInterface, QuDBusPluginInterface_iid)

#endif // QUDBUSPLUGININTERFACE_H
