#ifndef CUWSACTIONI_H
#define CUWSACTIONI_H

#include <cuthreadlistener.h>
#include <ws_source.h>

class CuDataListener;
class QString;
class QJsonDocument;


class CuWSActionI
{
public:

    enum Type { Reader = 0, Writer, ReaderConfig, WriterConfig, DbReadProp, DbWriteProp };

    /*! \brief virtual destructor, to make compilers happy
     */
    virtual ~CuWSActionI() {}

    /*! \brief return a WSSource object that stores information about the Tango source
     *
     * @return WSSource, an object storing information on the Tango source
     */
    virtual WSSource getSource() const  = 0;

    /*! \brief returns the type of CuWSActionI implemented
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

    virtual void start() = 0;

    virtual bool exiting() const = 0;

    virtual void stop() = 0;

    virtual void decodeMessage(const QJsonDocument& json) = 0;
};

#endif // CUWSACTIONI_H
