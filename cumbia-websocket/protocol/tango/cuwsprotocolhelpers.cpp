#include "cuwsprotocolhelpers.h"
#include "protocol/cuwsprotocolhelper_i.h"
#include "tango/cuwstangohelper.h"

CuWsProtocolHelpers::CuWsProtocolHelpers()
{
    m_helpersMap.insert("tango", new CuWsTangoHelper);
}

CuWsProtocolHelpers::~CuWsProtocolHelpers()
{
    foreach(ProtocolHelper_I* h, m_helpersMap.values())
        delete h;
    m_helpersMap.clear();
}

ProtocolHelper_I *CuWsProtocolHelpers::get(const QString &protonam) const
{
    return m_helpersMap[protonam];
}
