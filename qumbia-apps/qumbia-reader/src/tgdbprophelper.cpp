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
                bu[TTT::Device] = p.section("/", 0, 2).toStdString();  // bu["device"]
            }
            if(p.count("/") == 4) { // specific attribute property
                bu[TTT::Attribute] = p.section("/", 3, 3).toStdString();  // bu["attribute"]
                bu[TTT::Name] = p.section("/", -1).toStdString();  // bu["name"]
            }
            else if(p.count("/") == 3) // att props, all
            {
                bu[TTT::Attribute] = p.section("/", -1).toStdString();  // bu["attribute"]
            }
            else if(p.count("/") == 1) { // specific class prop
                bu[TTT::Class] = p.section("/", 0, 0).toStdString();  // bu["class"]
                bu[TTT::Name] = p.section('/', -1).toStdString();  // bu["name"]
            }
            else if(p.count("/") == 0 && p.endsWith(":")) { // class name
                // get all properties for the given class
                // - only "class" key is specified
                bu[TTT::Class] = p.section(":", 0, 0).toStdString();  // bu["class"]
            }
            else if(p.count(":")) { // specific device property
                bu[TTT::Device] = p.section(":", 0, 0).toStdString();  // bu["device"]
                if(!p.section(":", -1).isEmpty())
                    bu[TTT::Name] = p.section(":", -1).toStdString();  // bu["name"]
            }
            m_dbr = new CuTDbPropertyReader(p.toStdString(), ct);
            in.push_back(bu);
        }
        m_dbr->addListener(m_listener);
        m_dbr->get(in);
    }
}


#endif
