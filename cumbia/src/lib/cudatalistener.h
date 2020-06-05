#ifndef CUTANGOLISTENER_H
#define CUTANGOLISTENER_H

class CuData;
class CuTangoActionI;

#include <stdio.h>

class CuDataListenerPrivate;

/*! \brief abstract class defined for client libraries and applications to
 *         receive updates from *cumbia activities*
 *
 * This abstract class, defined in the *cumbia* base library, can be used
 * by clients to write classes that, implementing this interface, receive
 * updates from CuThreadListener objects when new data is available from
 * activities. It is not used by any class of the *cumbia base* library.
 *
 * \par Examples
 * \li *cumbia-qtcontrols* module widgets, such as QuLabel, QuLed, QuButton implement
 * the CuDataListener interface and are notified when new data is published
 * from an activity running in the background.
 *
 * \li *cumbia-tango*  module *CuTReader* class implements the CuThreadListener interface,
 *     and its CuThreadListener::onProgress and CuThreadListener::onResult methods.
 *     CuTReader::onResult invokes CuDataListener::onUpdate on every listener
 *     registered with CuTReader::addDataListener
 *
 * \par Note
 * Since v1.2.0 the invalid, invalidate and setValid methods are not used
 *
 */
class CuDataListener
{
public:

    CuDataListener();

    virtual ~CuDataListener();
 
    virtual void onUpdate(const CuData& data) = 0;

    virtual bool invalid() const;
    virtual bool invalidate();
    virtual void setValid();

private:
    CuDataListenerPrivate *d;
};

#endif // CUTANGOLISTENER_H
