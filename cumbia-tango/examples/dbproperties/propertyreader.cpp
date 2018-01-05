#include "propertyreader.h"
#include <cutdbpropertyreader.h>
#include <cumacros.h>
#include <cudata.h>
#include <algorithm>
#include <list>
#include <cuthreadfactoryimpl.h>
#include <cuthreadseventbridge.h>
#include <cumbiatango.h>

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
            CuData devpd("device", props[i].substr(0, cpos));
            devpd["name"] = props[i].substr(cpos + 1, std::string::npos);
            in_data.push_back(devpd);
        }
        else if(cnt == 3) {
            CuData devpd("device", props[i].substr(0, props[i].rfind('/')));
            if(cpos < std::string::npos) {
                devpd["attribute"] = props[i].substr(props[i].rfind('/') + 1, cpos - props[i].rfind('/') -1);
                devpd["name"] = props[i].substr(cpos + 1, std::string::npos);
            }
            else
                devpd["attribute"] = props[i].substr(props[i].rfind('/') + 1, cpos); // cpos == npos
            printf("\e[1;34mDO IN INGRESSO name %s att %s\e[0m\n", devpd["name"].toString().c_str(),
                    devpd["attribute"].toString().c_str());
            in_data.push_back(devpd);
        }
        else if(cnt == 0 && cpos < std::string::npos) { // class
            CuData cld("class", props[i].substr(0, cpos));
            cld["name"] = props[i].substr(cpos + 1);
            in_data.push_back(cld);
        }
    }
    CuTDbPropertyReader *pr = new CuTDbPropertyReader(id, m_ct);
    pr->addListener(this);
    pr->get(in_data);
    static_cast<CuEventLoopService*>(m_ct->getServiceProvider()->get(CuServices::EventLoop))->wait();
}

void PropertyReader::onUpdate(const CuData &data)
{
    pr_thread();
    printf("\e[1;32mPropertyReader.onUpdate! \n\e[0;36m%s\e[0m\n", data.toString().c_str());
    static_cast<CuEventLoopService*>(m_ct->getServiceProvider()->get(CuServices::EventLoop))->exit();
}
