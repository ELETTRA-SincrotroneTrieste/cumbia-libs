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
    CuServiceProvider *sp = m_ct->getServiceProvider();
    sp->registerService(CuServices::EventLoop, new CuEventLoopService());
    /* start the event loop in a separate thread (true param), where data from activities will be posted */
    static_cast<CuEventLoopService*>(sp->get(CuServices::EventLoop))->exec(true);
}

// test/device/1:description device property: two '/' and ':'
// test/device/1/double_scalar:values attribute properties: three '/' and one ':'
// TangoTest:Description class property: one '/'
void PropertyReader::get(const char *id, const std::vector<std::string> &props)
{
    /* start the event loop in a separate thread, where data from activities will be posted */
    std::vector<CuData> in_data;
    for(size_t i = 0; i < props.size(); i++) {
        size_t cnt = count(props[i].begin(), props[i].end(), '/');
        size_t cpos = props[i].find(':');
        if(cnt == 2 && cpos < std::string::npos)
        {
            CuData devpd(TTT::Device, props[i].substr(0, cpos));  // CuData devpd("device", props[i].substr(0, cpos)
            devpd[TTT::Name] = props[i].substr(cpos + 1, std::string::npos);  // devpd["name"]
            in_data.push_back(devpd);
        }
        else if(cnt == 3) {
            CuData devpd(TTT::Device, props[i].substr(0, props[i].rfind('/')));  // CuData devpd("device", props[i].substr(0, props[i].rfind('/')
            if(cpos < std::string::npos) {
                devpd[TTT::Attribute] = props[i].substr(props[i].rfind('/') + 1, cpos - props[i].rfind('/') -1);  // devpd["attribute"]
                devpd[TTT::Name] = props[i].substr(cpos + 1, std::string::npos);  // devpd["name"]
            }
            else
                devpd["attribute"] = props[i].substr(props[i].rfind('/') + 1, cpos); // cpos == npos

            in_data.push_back(devpd);
        }
        else if(cnt == 0 && cpos < std::string::npos) { // class
            CuData cld(TTT::Class, props[i].substr(0, cpos));  // CuData cld("class", props[i].substr(0, cpos)
            cld[TTT::Name] = props[i].substr(cpos + 1);  // cld["name"]
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
    if(data[TTT::Err].toBool())  // data["err"]
        printf("\n\033[1;31m** \033[0m error fetching properties: \033[1;31m%s\033[0m\n", data[TTT::Message].toString().c_str());  // data["msg"]
    else
        printf("\n\e[1;32m** %45s     VALUES\e[0m\n", "PROPERTIES");
    std::vector<std::string> plist = data["list"].toStringVector();
    for(size_t i = 0; i < plist.size(); i++)
        printf("\e[1;32m--\e[0m %55s \e[1;33m--> \e[0m%s\n", plist[i].c_str(), data[plist[i]].toString().c_str());
    exit();
}
