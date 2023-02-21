#ifndef QUACTIONEXTENSIONPLUGININTERFACE_H
#define QUACTIONEXTENSIONPLUGININTERFACE_H

#include <QObject>
#include <QStringList>
#include <cudata.h>
#include <vector>

class Cumbia;
class CumbiaPool;
class CuControlsReaderFactoryI;
class CuControlsFactoryPool;
class CuContextI;

class QuActionExtensionI {
  public:

    virtual ~QuActionExtensionI() {}

    virtual QString getName() const = 0;

    virtual CuData execute(const CuData& in, const CuContextI* ctx) = 0;

    virtual std::vector<CuData> execute(const std::vector<CuData>& in_list, const CuContextI* ctx) = 0;

    virtual QObject* get_qobject() = 0;

    virtual const CuContextI *getContextI() const = 0;

    virtual std::string message() const = 0;

    virtual bool error() const = 0;
};

class QuActionExtensionFactoryI
{
public:
    virtual ~QuActionExtensionFactoryI() {}

    virtual QuActionExtensionI *create(const QString& name, const CuContextI *ctx) = 0;
};

/** \brief Interface for a plugin
 *
 */
class QuActionExtensionPluginInterface
{
public:
    virtual ~QuActionExtensionPluginInterface() { }

    virtual void registerExtension(const QString& name, QuActionExtensionI *ae) = 0;

    virtual QStringList extensions() const = 0;

    virtual QuActionExtensionI *getExtension(const QString& name) = 0;

    virtual QuActionExtensionFactoryI *getExtensionFactory() = 0;

    /*! \brief removes all registered extensions
     */
    virtual void clear() = 0;
};

#define QuActionExtensionPluginInterface_iid "eu.elettra.qutils.QuActionExtensionPluginInterface"

Q_DECLARE_INTERFACE(QuActionExtensionPluginInterface, QuActionExtensionPluginInterface_iid)

#endif // QUACTIONEXTENSIONPLUGININTERFACE_H
