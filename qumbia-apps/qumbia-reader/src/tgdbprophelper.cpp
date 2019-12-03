#ifdef QUMBIA_TANGO_CONTROLS_VERSION

#include "tgdbprophelper.h"
#include <cumbiapool.h>
#include <cutdbpropertyreader.h>
#include <cumbiatango.h>
#include <cudatalistener.h>
#include <QtDebug>

TgDbPropHelper::TgDbPropHelper(CuDataListener *li)
{
    m_listener = li;
    m_dbr = nullptr;
}

TgDbPropHelper::~TgDbPropHelper() {
    if(m_dbr)
        delete m_dbr;
}

void TgDbPropHelper::get(CumbiaPool *cu_p, const QStringList &props)
{
    std::vector<CuData> in;
    CumbiaTango *ct = static_cast<CumbiaTango *>(cu_p->get("tango"));
    if(ct) {
        CuData bu; // att property
        foreach(QString p, props) {
            if(p.count("/") > 1 && p.count(":") == 0) {
                bu["device"] = p.section("/", 0, 2).toStdString();
            }
            if(p.count("/") == 4) {
                bu["attribute"] = p.section("/", 3, 3).toStdString();
                bu["name"] = p.section("/", -1).toStdString();
            }
            else if(p.count("/") == 3) // att props, all
            {
                bu["attribute"] = p.section("/", -1).toStdString();
            }
            else if(p.count(":")) { // device
                bu["device"] = p.section(":", 0, 0).toStdString();
                bu["name"] = p.section(":", -1).toStdString();
            }
            printf("TgDbPropHelper.get: tg_prop: %s\n", bu.toString().c_str());
            m_dbr = new CuTDbPropertyReader(p.toStdString(), ct);
            in.push_back(bu);
        }
        m_dbr->addListener(m_listener);
        m_dbr->get(in);
    }
}


#endif