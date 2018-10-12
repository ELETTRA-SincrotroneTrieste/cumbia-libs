#ifndef CUCONTROLSWRITERA_H
#define CUCONTROLSWRITERA_H

#include <string>
#include <vector>
#include <assert.h>
#include <cuvariant.h>
#include <cudata.h>

class QString;
class Cumbia;
class CuDataListener;
class QStringList;

/*! @private */
class CuControlsWriterAbstractPrivate
{
public:
    Cumbia* cumbia;
    CuDataListener *listener;
    CuVariant argins;
    CuData configuration;
};

/*! \brief abstract class defining an interface for cumbia-qtcontrols writers
 *
 * This abstract class defines an interface for writers. cumbia-qtcontrols writers
 * do not know about the engine used to connect and write to the control system.
 *
 * The method getCumbia returns a pointer to the Cumbia engine currently used by
 * the writer.
 * getDataListener returns a pointer to the CuDataListener that is listening for
 * updates (usually, write errors).
 * The CuDataListener and the Cumbia engine are passed to CuControlsWriterA
 * in the constructor.
 * The setArgs and getArgs methods set and return a CuVariant that can be used
 * to provide input arguments for the write operations.
 *
 * All the other methods are pure virtual ones that must be implemented by the subclass.
 */
class CuControlsWriterA
{
public:

    /*! the class constructor initialises the writer.
     *
     * @param c a pointer to the Cumbia instance in use.
     * @param l a CuDataListener that will receive updates.
     *
     * \par Note both parameters must be not null
     */
    CuControlsWriterA(Cumbia *c, CuDataListener *l)
    {
        assert(c != NULL && l != NULL);
        d = new CuControlsWriterAbstractPrivate;
        d->cumbia = c;
        d->listener = l;
    }

    virtual ~CuControlsWriterA()
    {
        delete d;
    }

    /*!  \brief returns the name of the target
     *
     * @return the name of the target configured with setTarget.
     *
     * If wildcards were used in setTarget, target will return the complete target,
     * with the substitutions.
     */
    virtual QString target() const = 0;

    virtual void clearTarget() = 0;

    /*! \brief connect the specified target to the control system engine.
     *
     * @param s the name of the target. The syntax depends on the engine.
     *
     * Subclasses can check the parameter to ensure that its form is valid.
     */
    virtual void setTarget(const QString &s) = 0;

    /*! \brief execute the target specified with setTarget
     *
     * Subclasses will execute the write operation in this method, according
     * to the control system specific engine
     */
    virtual void execute() = 0;

    /*! \brief send data to the writer
     *
     * @param d a bundle of key/value pairs defining the data to send to the writer
     *
     * @see getData
     */
    virtual void sendData(const CuData& d) = 0;

    /*! \brief get data from the writer
     *
     * @param d_ino a bundle of key/value pairs defining the data to send to the writer
     *        and containing the results filled by the writer once the function
     *        returns
     */
    virtual void getData(CuData& d_ino) const = 0;

    /*! \brief returns the arguments set with setArgs
     *
     * @return a CuVariant storing the value(s) that can be used as input
     *         argument(s) by execute
     *
     * @see execute
     * @see CuVariant
     */
    CuVariant getArgs() const { return d->argins; }

    /*! \brief sets the value that can be used as input argument
     *
     * @return a CuVariant storing the value that can be used as input
     *         argument by execute
     *
     * @see execute
     */
    void setArgs(const CuVariant &args) { d->argins = args; }

    /*! \brief returns a pointer to the Cumbia implementation that was passed to the
     *         class constructor
     *
     * @return a pointer to the Cumbia implementation used by the writer
     */
    Cumbia *getCumbia() const { return d->cumbia; }

    /*! \brief return the CuDataListener that was specified in the constructor.
     *
     * @return the CuDataListener that receives new data updates.
     */
    CuDataListener *getDataListener() const { return d->listener; }

    /*! \brief returns the CuData previously saved with saveConfiguration
     *
     * Writers will typically load configuration from a database at creation time and store it
     * across following execute calls.
     *
     * @see saveConfiguration
     */
    CuData getConfiguration() const { return d->configuration; }

    /*! \brief when writer configuration is fetched (e.g. from a database), it can be saved
     *         in order to avoid database calls during the execution
     *
     * @param da configuration data
     */
    void saveConfiguration(const CuData& da) { d->configuration = da; }

private:
    CuControlsWriterAbstractPrivate *d;
};

#endif // CUCONTROLSWRITERI_H
