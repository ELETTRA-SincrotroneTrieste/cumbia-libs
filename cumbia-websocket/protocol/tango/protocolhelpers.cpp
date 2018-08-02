#include "protocolhelpers.h"
#include "protocol/protocolhelper_i.h"
#include "tango/tangohelper.h"

ProtocolHelpers::ProtocolHelpers()
{
    m_helpersMap.insert("tango", new TangoHelper);
}

ProtocolHelpers::~ProtocolHelpers()
{
    foreach(ProtocolHelper_I* h, m_helpersMap.values())
        delete h;
    m_helpersMap.clear();
}

ProtocolHelper_I *ProtocolHelpers::get(const QString &protonam) const
{
    return m_helpersMap[protonam];
}
