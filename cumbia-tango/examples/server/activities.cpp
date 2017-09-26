#include "activities.h"
#include <cumacros.h>
#include <math.h>

CuTActivity::CuTActivity(const CuData &token) : CuContinuousActivity(token)
{

}

int CuTActivity::getType() const
{
    return CuActivity::Continuous;
}

void CuTActivity::init()
{

}

void CuTActivity::execute()
{
    CuData d;
    std::string val(this->getToken()["activity"].toString());
    std::string device = getToken()["device"].toString();
    d["device"] = device;
    try{
        Tango::DeviceProxy *dev = new Tango::DeviceProxy(device);
        Tango::DeviceAttribute da = dev->read_attribute("double_scalar");
        Tango::DevDouble dd;
        da >> dd;
        d["double_scalar"] = dd;
        printf("double scalar is %f\n", dd);
    }
    catch(Tango::DevFailed& e)
    {
        d["error"] = "Error reading or connecting to " + device;
    }
    publishResult(d);
}

void CuTActivity::onExit()
{

}

bool CuTActivity::matches(const CuData &token) const
{
    return token == this->getToken();
}

WriteActivity::WriteActivity(const CuData &input) : CuIsolatedActivity(input)
{

}

WriteActivity::~WriteActivity()
{
    pdelete("~WriteActivity %p %s", this, getToken().toString().c_str());
}

void WriteActivity::init()
{

}

void WriteActivity::execute()
{
    pbgreen2("WriteActivity.execute IN >>>>>>>>>>>>>>>>>>>>>");
    CuData in = getToken();
    std::string device = in["device"].toString();
    double val = in["double_scalar"].toDouble();
    try{
        Tango::DeviceProxy *dev = new Tango::DeviceProxy(device);
        Tango::DeviceAttribute da("double_scalar", val);
        dev->write_attribute(da);
        in["success"] = true;
    }
    catch(Tango::DevFailed& e)
    {
        in["error"] = "Error writing or connecting to " + device;
        in["success"] = false;
    }
    publishResult(in);
    pbgreen2("WriteActivity.execute OUT <<<<<<<<<<<<<<<<<<<<<<<<<<");
}

void WriteActivity::onExit()
{

}

bool WriteActivity::matches(const CuData &token) const
{
    return token == getToken();
}

void WriteActivity::event(CuActivityEvent *e)
{

}
