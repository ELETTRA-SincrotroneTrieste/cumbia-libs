#include "propertyreader.h"
#include <cutdbpropertyreader.h>
#include <cumacros.h>
#include <cudata.h>
#include <algorithm> // for find in vector
#include <list>
#include <cuthreadfactoryimpl.h>
#include <cuthreadseventbridge.h>
#include <cumbiatango.h>
#include <cuserviceprovider.h>


PropertyReader::PropertyReader()
{
    m_ct = new CumbiaTango(new CuThreadFactoryImpl(), new CuThreadsEventBridgeFactory());
    m_ct->getServiceProvider()->registerService(CuServices::EventLoop, new CuEventLoopService());
    /* start the event loop in a separate thread (true param), where data from activities will be posted */
    static_cast<CuEventLoopService*>(m_ct->getServiceProvider()->get(CuServices::EventLoop))->exec(true);
}

// test/device/1:description device property: two '/' and ':'
// test/device/1/double_scalar:values attribute properties: three '/' and one ':'
// TangoTest:Description class property: one '/'
void PropertyReader::get(const char *id, const std::vector<std::string> &props)
{
    /* start the event loop in a separate thread, where data from activities will be posted */
    std::list<CuData> in_data;
    for(size_t i = 0; i < props.size(); i++) {
        size_t cnt = count(props[i].begin(), props[i].end(), '/');
        size_t cpos = props[i].find(':');
        if(cnt == 2 && cpos < std::string::npos)
        {
            CuData devpd(CuXDType::Device, props[i].substr(0, cpos));
            devpd[CuDType::Name] = props[i].substr(cpos + 1, std::string::npos);
            in_data.push_back(devpd);
        }
        else if(cnt == 3) {
            CuData devpd(CuXDType::Device, props[i].substr(0, props[i].rfind('/')));
            if(cpos < std::string::npos) {
                devpd[CuXDType::Point] = props[i].substr(props[i].rfind('/') + 1, cpos - props[i].rfind('/') -1);
                devpd[CuDType::Name] = props[i].substr(cpos + 1, std::string::npos);
            }
            else
                devpd[CuXDType::Point] = props[i].substr(props[i].rfind('/') + 1, cpos); // cpos == npos

            in_data.push_back(devpd);
        }
        else if(cnt == 0 && cpos < std::string::npos) { // class
            CuData cld(CuDType::Class, props[i].substr(0, cpos));
            cld[CuDType::Name] = props[i].substr(cpos + 1);
            in_data.push_back(cld);
        }
    }
    CuTDbPropertyReader *pr = new CuTDbPropertyReader(id, m_ct);
    pr->addListener(this);
    pr->get(in_data);
    static_cast<CuEventLoopService*>(m_ct->getServiceProvider()->get(CuServices::EventLoop))->wait();
    delete m_ct;
    delete pr;
}

void PropertyReader::exit()
{

    static_cast<CuEventLoopService*>(m_ct->getServiceProvider()->get(CuServices::EventLoop))->exit();
}

void PropertyReader::onUpdate(const CuData &data)
{
    pr_thread();
    if(data[CuDType::Err].toBool())
        perr("PropertyReader.onUpdate: error fetching properties: %s", data[CuDType::Message].toString().c_str());
    else
        printf("\n\e[1;32m** %45s     VALUES\e[0m\n", "PROPERTIES");
    std::vector<std::string> plist = data[CuDType::Value].toStringVector();
    for(size_t i = 0; i < plist.size(); i++)
        printf("\e[1;32m--\e[0m %55s \e[1;33m--> \e[0m%s\n", plist[i].c_str(), data[plist[i]].toString().c_str());
    exit();
}
