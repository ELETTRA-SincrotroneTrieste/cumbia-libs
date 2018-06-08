#include "cutdbpropertyreader.h"
#include "cugettdbpropactivity.h"
#include <set>
#include <cudatalistener.h>
#include <cuserviceprovider.h>
#include <culog.h>
#include <cuactivity.h>
#include <list>
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
    bool exit;
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
 * \li "device" --> "tango/device/name"
 * \li "name" --> "property_name"
 *
 * \subsection Attribute properties
 *
 *  The three following key/values must be set:
 * \li "device"  --> "tango/device/name"
 * \li "attribute" --> "attribute_name"
 * \li "name" --> "property_name"
 *
 * \note If no "name" is provided, all the device attribute properties and their values are retrieved.
 *
 * \subsection Class properties.
 *
 * CuData must be configured as follows:
 *
 * \li "class" --> "class_name"
 * \li "name" --> "property_name"
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
 * CuData devpd("device", "test/device/1");
 * devpd[CuDType::Name] = "description";
 * in_data.push_back(devpd);
 * // attribute property values from the "double_scalar" attribute
 * CuData apd("device", "test/device/2");
 * apd["attribute"] = "double_scalar";
 * apd[CuDType::Name] = "values";
 * in_data.push_back(apd);
 * // class property
 * CuData cld("class", "TangoTest");
 * cld[CuDType::Name] = "ProjectTitle";
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
void CuTDbPropertyReader::get(const std::list<CuData> &in_data)
{
    CuData thread_tok;
    thread_tok[CuDType::Thread] = d->id;
    d->activity = new CuGetTDbPropActivity(in_data);
    const CuThreadsEventBridgeFactory_I &bf = *(d->cumbia_t->getThreadEventsBridgeFactory());
    const CuThreadFactoryImplI &fi = *(d->cumbia_t->getThreadFactoryImpl());
    d->cumbia_t->registerActivity(d->activity, this, thread_tok, fi, bf);
}

/** \brief register a CuDataListener that will be notified when data is ready.
 *
 */
void CuTDbPropertyReader::addListener(CuDataListener *l)
{
    d->listeners.insert(l);
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
}

CuData CuTDbPropertyReader::getToken() const
{
    CuData da;
    da[CuDType::Thread] = d->id;
    return da;
}

