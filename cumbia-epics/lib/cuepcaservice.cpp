#include "cuepcaservice.h"
#include <mutex>
#include <cumacros.h>
#include "epsource.h"
#include <cadef.h>

CuEpCAService::CuEpCAService()
{
    m_result = ca_context_create(ca_enable_preemptive_callback);
    if (m_result != ECA_NORMAL) {
        perr("CA error %s occurred while trying "
                "to start channel access.\n", ca_message(m_result));
    }
}

CuEpCAService::~CuEpCAService()
{
    ca_context_destroy();
    pdelete("~CuDeviceFactoryService deleted %p", this);
}

string CuEpCAService::getStatus() const
{
    return std::string(ca_message(m_result));
}

std::::string CuEpCAService::getName() const
{
    return "EpicsChannelAccessService";
}

CuServices::Type CuEpCAService::getType() const
{
    return static_cast<CuServices::Type> (CuEpicsChannelAccessServiceType);
}
