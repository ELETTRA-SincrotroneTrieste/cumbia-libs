#include "cuhttpprotocolhelpers.h"
#include "protocol/cuhttpprotocolhelper_i.h"
#include "tango/cuhttptangohelper.h"

CuHttpProtocolHelpers::CuHttpProtocolHelpers()
{
    m_helpersMap.insert("tango", new CuHttpTangoHelper);
}

CuHttpProtocolHelpers::~CuHttpProtocolHelpers()
{
    foreach(ProtocolHelper_I* h, m_helpersMap.values())
        delete h;
    m_helpersMap.clear();
}

ProtocolHelper_I *CuHttpProtocolHelpers::get(const QString &protonam) const
{
    return m_helpersMap[protonam];
}
