#ifndef QUACTIONEXTENSIONPLUGININTERFACE_H
#define QUACTIONEXTENSIONPLUGININTERFACE_H

#include <QObject>
#include <cudata.h>
#include <QString>

class Cumbia;
class CumbiaPool;
class CuControlsReaderFactoryI;
class CuControlsFactoryPool;
class QString;
class QStringList;
class CuContextI;

class QuActionExtensionI {
  public:

    virtual ~QuActionExtensionI() {}

    virtual QString getName() const = 0;

    virtual CuData execute(const CuData& in) = 0;

    virtual QObject* get_qobject() = 0;
};

class QuActionExtensionFactoryI
{
public:
    virtual ~QuActionExtensionFactoryI() {}

    virtual QuActionExtensionI *create(const QString& name, CuContextI *ctx) = 0;
};

/** \brief Interface for a plugin
 *
 */
class QuActionExtensionPluginInterface
{
public:
    virtual ~QuActionExtensionPluginInterface() { }

//    /** \brief Initialise the multi reader with the desired engine and the read mode.
//     *
//     * @param cumbia a reference to the cumbia  implementation
//     * @param r_fac the engine reader factory
//     * @param manual_mode_code the value to be passed to the reading engine in order to make it work in
//     *        manual mode (no polling, no event refresh mode) and perform sequential reads or a negative
//     *        number to work in parallel mode.
//     */
//    virtual void init(Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac, int manual_mode_code) = 0;

//    /** \brief Initialise the multi reader with mixed engine mode and the read mode.
//     *
//     * @param cumbia a reference to the CumbiaPool engine chooser
//     * @param r_fac the CuControlsFactoryPool factory chooser
//     * @param manual_mode_code the value to be passed to the reading engine in order to make it work in
//     *        manual mode (no polling, no event refresh mode) and perform sequential reads or a negative
//     *        number to work in parallel mode.
//     */
//    virtual void init(CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool, int manual_mode_code) = 0;



//    /** \brief To provide the necessary signals aforementioned, the implementation must derive from
//     *         Qt QObject. This method returns the subclass as a QObject, so that the client can
//     *         connect to the multi reader signals.
//     *
//     * @return The object implementing QuMultiReaderPluginInterface as a QObject.
//     */
//    virtual const QObject* get_qobject() const = 0;

    virtual void registerExtension(const QString& name, QuActionExtensionI *ae) = 0;

    virtual QStringList extensions() const = 0;

    virtual QuActionExtensionI *getExtension(const QString& name) = 0;

    virtual QuActionExtensionFactoryI *getExtensionFactory() = 0;
};

#define QuActionExtensionPluginInterface_iid "eu.elettra.qutils.QuActionExtensionPluginInterface"

Q_DECLARE_INTERFACE(QuActionExtensionPluginInterface, QuActionExtensionPluginInterface_iid)

#endif // QUACTIONEXTENSIONPLUGININTERFACE_H
