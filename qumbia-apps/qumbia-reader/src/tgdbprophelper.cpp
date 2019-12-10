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
            if(p.count("/") == 4) { // specific attribute property
                bu["attribute"] = p.section("/", 3, 3).toStdString();
                bu["name"] = p.section("/", -1).toStdString();
            }
            else if(p.count("/") == 3) // att props, all
            {
                bu["attribute"] = p.section("/", -1).toStdString();
            }
            else if(p.count("/") == 1) { // specific class prop
                bu["class"] = p.section("/", 0, 0).toStdString();
                bu["name"] = p.section('/', -1).toStdString();
            }
            else if(p.count("/") == 0 && p.endsWith(":")) { // class name
                // get all properties for the given class
                // - only "class" key is specified
                bu["class"] = p.section(":", 0, 0).toStdString();
            }
            else if(p.count(":")) { // specific device property
                bu["device"] = p.section(":", 0, 0).toStdString();
                if(!p.section(":", -1).isEmpty())
                    bu["name"] = p.section(":", -1).toStdString();
            }
            m_dbr = new CuTDbPropertyReader(p.toStdString(), ct);
            in.push_back(bu);
        }
        m_dbr->addListener(m_listener);
        m_dbr->get(in);
    }
}


#endif
