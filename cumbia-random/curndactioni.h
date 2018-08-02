#ifndef CURNDACTIONI_H
#define CURNDACTIONI_H

#include <cuthreadlistener.h>
#include <rnd_source.h>

class CuDataListener;

class CuRNDActionI : public CuThreadListener
{
public:

    enum Type { Reader = 0, Writer, AttConfig, DbReadProp, DbWriteProp };

    CuRNDActionI();


    /*! \brief virtual destructor, to make compilers happy
     */
    virtual ~CuRNDActionI() {}


    /*! \brief return a RNDSource object that stores information about the Tango source
     *
     * @return RNDSource, an object storing information on the Tango source
     */
    virtual RNDSource getSource() const  = 0;

    /*! \brief returns the type of CuRNDActionI implemented
     *
     * @return one of the values from the Type enum
     */
    virtual Type getType() const = 0;

    /*! \brief insert a CuDataListener to the list of listeners of data updates
     *
     * @param l a CuDataListener that receives notifications when data is ready,
     *        through CuDataListener::onUpdate
     */
    virtual void addDataListener(CuDataListener *l) = 0;

    /*! \brief remove a CuDataListener from the list of listeners of data updates
     *
     * @param l a CuDataListener that will be removed from the list of listeners
     */
    virtual void removeDataListener(CuDataListener *l) = 0;

    /*! \brief return the number of registered data listeners
     *
     * @return the number of data listeners currently installed
     */
    virtual size_t dataListenersCount() = 0;

    /*! \brief the method where activities are created and registered
     *
     * This is the place where activities are created and registered to Cumbia through
     * the method Cumbia::registerActivity.
     *
     * \par Examples
     * \li CuTReader::start
     * \li CuTWriter::start
     *
     * See also Cumbia::registerActivity
     */
    virtual void start() = 0;

    /*! \brief the method where activities are unregistered from Cumbia
     *
     * This is the place where activities are unregistered from Cumbia by the
     * Cumbia::unregisterActivity method
     *
     * \par Examples
     * \li CuTReader::stop
     * \li CuTWriter::stop
     *
     * See also Cumbia::unregisterActivity
     *
     */
    virtual void stop() = 0;

    /*! \brief subclasses will implement the necessary code to set data on themselves or to the
     *         activities
     *
     * @param data a CuData compiled with the necessary fields accepted by the specific implementation
     *
     * The data can be set locally on the object implementing this interface or can be forwarded to the
     * activity by means of Cumbia::postEvent carrying an argument derived from CuActivityEvent.
     * See CuTReader::sendData for an example
     *
     * \par Send a request and receive a response *asynchronously*
     * Use sendData *in conjunction with* CuDataListener::onUpdate, as described in the getData
     * documentation.
     */
    virtual void sendData(const CuData& data) = 0;

    /*! \brief subclasses will implement the necessary code to analyze the request stored in the input
     *         argument and fill it in with output data
     *
     * @param data a CuData compiled with the necessary fields accepted by the specific implementation
     *        as input argument. data will contain additional output fields filled in by the method
     *        when it returns.
     *
     * \par note
     * getData must be synchronous: it's called with input arguments (d_inout) and they are filled in
     * within the same method in the same thread. *Asynchronous* data fetch from an activity executed
     * in a separate thread can be accomplished by sendData *in combination with*
     * CuDataListener::onUpdate. There, received data will have to be recognised as coming from
     * a specific request and treated accordingly.
     */
    virtual void getData(CuData& d_inout) const = 0;

    /*! \brief returns true if the action has been stopped and it's exiting
     *
     * @return true if the action is exiting (normally, stop has been called), false otherwise
     *
     * Used by CuActionFactoryService::unregisterAction to check whether the action is
     * still running before unregistering.
     */
    virtual bool exiting() const = 0;

    // CuThreadListener interface
public:
    virtual void onProgress(int step, int total, const CuData &data);
    virtual void onResult(const CuData &data);
    virtual void onResult(const std::vector<CuData> &datalist);
    virtual CuData getToken() const;
};

#endif // CURNDACTIONI_H
