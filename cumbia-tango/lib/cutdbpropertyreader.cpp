#include "cutdbpropertyreader.h"
#include "cugettdbpropactivity.h"
#include <set>
#include <cudatalistener.h>
#include <cuserviceprovider.h>
#include <culog.h>
#include <cuactivity.h>
#include <cumbiatango.h>
#include <cuthreadfactoryimpl_i.h>
#include <cuthreadseventbridgefactory_i.h>

class CuTDbPropertyReaderPrivate
{
public:
    std::set<CuDataListener *> listeners;
    std::list<CuData> in_props;
    std::string id;
    CumbiaTango *cumbia_t;
    CuGetTDbPropActivity *activity;
    bool delete_later, done;
    CuConLogImpl li;
    CuLog log;
};

/** \brief The class constructor.
 *
 * @param id any identifier as a std string
 * @param cumbia_tango a reference to a CumbiaTango instance, necessary to configure the property reader
 *        activity with proper CuThreadsEventBridgeFactory_I and CuThreadFactoryImplI implementations.
 *
 * @see get
 * @see addListener
 *
 */
CuTDbPropertyReader::CuTDbPropertyReader(const std::string &id, CumbiaTango *cumbia_t)
{
    d = new CuTDbPropertyReaderPrivate;
    d->cumbia_t = cumbia_t;
    d->id = id;
    d->delete_later = d->done = false;
}

CuTDbPropertyReader::~CuTDbPropertyReader()
{
    pdelete("~CuTDbPropertyReader %p", this);
    delete d;
}

/** \brief Get a list of device, class or attribute properties from the Tango database
 *
 * @param in_data a list of CuData bundles defining the properties to fetch.
 *
 * \section Format of the input data.
 *
 * Every element of in_data describes the property you want to get.
 *
 * \subsection Device properties
 *
 * To get a device property you must put into the CuData bundle the following key/value pairs:
 * \li CuDType::Device --> "tango/device/name"
 * \li CuDType::name --> "property_name"
 *
 * \subsection Attribute properties
 *
 *  The three following key/values must be set:
 * \li CuDType::Device  --> "tango/device/name"
 * \li CuDType::Attribute --> "attribute_name"
 * \li CuDType::name --> "property_name"
 *
 * \note If no CuDType::name is provided, all the device attribute properties and their values are retrieved.
 *
 * \subsection Class properties.
 *
 * CuData must be configured as follows:
 *
 * \li CuDType::Class --> "class_name"
 * \li CuDType::name --> "property_name"
 *
 * \subsection Tutorial
 *
 * There is a cumbia tutorial dedicated to getting properties from the Tango database:
 * \ref Writing an activity to read Tango device, attribute and class properties.
 *
 * \par Example
 * This code snippet fetches some properties of different kind:
 *
 * \code
 * std::list<CuData> in_data;
 * // device property
 * CuData devpd(CuDType::Device, "test/device/1");
 * devpd[CuDType::name] = CuDType::Description;
 * in_data.push_back(devpd);
 * // attribute property values from the "double_scalar" attribute
 * CuData apd(CuDType::Device, "test/device/2");
 * apd[CuDType::Attribute] = "double_scalar";
 * apd[CuDType::name] = "values";
 * in_data.push_back(apd);
 * // class property
 * CuData cld(CuDType::Class, "TangoTest");
 * cld[CuDType::name] = "ProjectTitle";
 * in_data.push_back(cld);
 *
 * // m_ct is a reference to a CumbiaTango object properly instantiated before.
 * CuTDbPropertyReader *pr = new CuTDbPropertyReader("getprop", m_ct);
 * pr->addListener(this); // this class implements CuDataListener
 * pr->get(in_data); // will receive results within the onUpdate method
 *
 * \endcode
 *
 * @see addListener
 */
void CuTDbPropertyReader::get(const std::vector<CuData> &in_data)
{
    CuData thread_tok("id", d->id);
    const std::string& thto = "cutdbprop_r:" + d->id;
    d->activity = new CuGetTDbPropActivity(in_data);
    const CuThreadsEventBridgeFactory_I &bf = *(d->cumbia_t->getThreadEventsBridgeFactory());
    const CuThreadFactoryImplI &fi = *(d->cumbia_t->getThreadFactoryImpl());
    d->cumbia_t->registerActivity(d->activity, this, thto, fi, bf);
}

/*!
 * \brief CuTDbPropertyReader::cancel tries to unregister the activity and removes all listeners
 *
 * Listeners can be deleted after cancel is called, since *onUpdate* will not be invoked after.
 * Nonetheless, cancel is not enough to ensure this object *can be safely deleted*. If you intend to
 * *cancel and delete* the CuTDbPropertyReader, call deleteLater after cancel.
 */
void CuTDbPropertyReader::cancel()
{
    d->listeners.clear();
    if(d->activity) {
        d->cumbia_t->unregisterActivity(d->activity);
        d->activity = nullptr;
    }
}

/** \brief register a CuDataListener that will be notified when data is ready.
 *
 */
void CuTDbPropertyReader::addListener(CuDataListener *l)
{
    d->listeners.insert(l);
}

/*!
 * \brief CuTDbPropertyReader::removeListener removes the given listener
 * \param l a pointer to the listener to be removed
 */
void CuTDbPropertyReader::removeListener(CuDataListener *l)
{
    d->listeners.erase(l);
}

/*!
 * \brief CuTDbPropertyReader::deleteLater schedule object auto destruction
 *
 * If the result has already been delivered, the object is immediately deleted.
 * This object will be deleted right after result delivery otherwise.
 *
 * \par Warnings
 * 1. Do not delete the listener set with addListener without previously calling either
 * removeListener or cancel.
 *
 * 2. Do not directly *delete* a CuTDbPropertyReader unless you are really sure that
 * onResult has been called.
 *
 * @see cancel
 */
void CuTDbPropertyReader::deleteLater()
{
    if(d->done)
        delete this;
}

/** This is not used
 */
void CuTDbPropertyReader::onProgress(int step, int total, const CuData &data)
{

}

void CuTDbPropertyReader::onResult(const CuData &data)
{
    for(std::set<CuDataListener *>::const_iterator it = d->listeners.begin(); it != d->listeners.end(); ++it)
        (*it)->onUpdate(data);
    d->done = true;
    if(d->delete_later)
        delete this;
}

CuData CuTDbPropertyReader::getToken() const
{
    return CuData("id", d->id);
}

void CuTDbPropertyReader::onResult(const std::vector<CuData> &datalist)
{
    (void) datalist;
}

