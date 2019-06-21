#include "cutango-world-config.h"

class CuTangoWorldConfigPrivate
{
public:
    std::unordered_map<Tango::DevState, std::string> stateStrings;
    std::unordered_map<Tango::DevState, std::string>  stateColors;

    std::unordered_map<Tango::AttrQuality, std::string> qualityStrings;
    std::unordered_map<Tango::AttrQuality, std::string> qualityColors;

    std::unordered_map<bool, std::string> successColors;

    /* non standard device servers with non standard "values" attribute property name */
    std::string valueAttrPropName;
};

CuTangoWorldConfig::CuTangoWorldConfig()
{
    d = new CuTangoWorldConfigPrivate;
    initStateColorsAndStrings();
    initQualityColorsAndStrings();

    d->successColors[true] = "dark_green";
    d->successColors[false] = "red";
    d->valueAttrPropName = "values";
}

CuTangoWorldConfig::~CuTangoWorldConfig()
{
    delete d;
}

void CuTangoWorldConfig::initStateColorsAndStrings()
{
    d->stateStrings[Tango::ON] =     "ON";
    d->stateStrings[Tango::OFF] =    "OFF";
    d->stateStrings[Tango::CLOSE] =  "CLOSED";
    d->stateStrings[Tango::OPEN] =   "OPENED";
    d->stateStrings[Tango::INSERT]     = "INSERT";
    d->stateStrings[Tango::EXTRACT]    = "EXTRACT";
    d->stateStrings[Tango::MOVING]     = "MOVING";
    d->stateStrings[Tango::STANDBY]    = "STANDBY";
    d->stateStrings[Tango::FAULT]      = "FAULT";
    d->stateStrings[Tango::INIT]       = "INIT";
    d->stateStrings[Tango::RUNNING]    = "RUNNING";
    d->stateStrings[Tango::ALARM]      = "ALARM";
    d->stateStrings[Tango::DISABLE]    = "DISABLE";
    d->stateStrings[Tango::UNKNOWN]    = "UNKNOWN";

    d->stateColors[Tango::ON]          = "green";
    d->stateColors[Tango::OFF]         = "white";
    d->stateColors[Tango::CLOSE]       = "white1";
    d->stateColors[Tango::OPEN]        = "white2";
    d->stateColors[Tango::INSERT]      = "white3";
    d->stateColors[Tango::EXTRACT]     = "white4";
    d->stateColors[Tango::MOVING]      = "blue";
    d->stateColors[Tango::STANDBY]     = "yellow";
    d->stateColors[Tango::FAULT]       = "red";
    d->stateColors[Tango::INIT]        = "blue";
    d->stateColors[Tango::RUNNING]     = "green";
    d->stateColors[Tango::ALARM]       = "orange";
    d->stateColors[Tango::DISABLE]     = "gray";
    d->stateColors[Tango::UNKNOWN]     = "darkGray";
}

void CuTangoWorldConfig::initQualityColorsAndStrings()
{
    d->qualityStrings[Tango::ATTR_VALID] = "ATTR_VALID";
    d->qualityStrings[Tango::ATTR_INVALID] = "ATTR_INVALID";
    d->qualityStrings[Tango::ATTR_ALARM] = "ATTR_ALARM";
    d->qualityStrings[Tango::ATTR_CHANGING] = "ATTR_CHANGING";
    d->qualityStrings[Tango::ATTR_WARNING] = "ATTR_WARNING";
    d->qualityColors[Tango::ATTR_VALID] = "white";
    d->qualityColors[Tango::ATTR_INVALID] = "gray";
    d->qualityColors[Tango::ATTR_ALARM] = "red";
    d->qualityColors[Tango::ATTR_CHANGING] = "blue";
    d->qualityColors[Tango::ATTR_WARNING] = "orange";
}

void CuTangoWorldConfig::setStateColors(const std::vector<string> &v)
{
    for(int i = 0; i < v.size(); i++)
    {
        Tango::DevState ds = (Tango::DevState)(i);
        d->stateColors[ds] = v[i];
    }
}

const string CuTangoWorldConfig::qualityColor(Tango::AttrQuality q) const
{
    if(d->qualityColors.find(q) != d->qualityColors.end())
        return d->qualityColors[q];
    return "gray";
}

void CuTangoWorldConfig::setQualityColor(Tango::AttrQuality q, const std::string& c)
{
    d->qualityColors[q] = c;
}

int CuTangoWorldConfig::qualityColorCount() const
{
    return d->qualityColors.size();
}

void CuTangoWorldConfig::setQualityString(Tango::AttrQuality q, std::string s)
{
    d->qualityStrings[q] = s;
}

std::string CuTangoWorldConfig::qualityString(Tango::AttrQuality q) const
{
    if(d->qualityStrings.find(q) != d->qualityStrings.end())
        return d->qualityStrings[q];
    return "UNDEFINED QUALITY";
}

std::unordered_map<Tango::DevState, string> CuTangoWorldConfig::stateStrings()
{
    return d->stateStrings;
}

std::unordered_map<Tango::DevState, string> CuTangoWorldConfig::stateColorNames()
{
    return d->stateColors;
}

std::unordered_map<Tango::AttrQuality, string> CuTangoWorldConfig::qualityStrings()
{
    return d->qualityStrings;
}

std::unordered_map<Tango::AttrQuality, string> CuTangoWorldConfig::qualityColorNames()
{
    return d->qualityColors;
}

void CuTangoWorldConfig::setOverrideValuesAttributePropertyName(const string &name)
{
    d->valueAttrPropName = name;
}

string CuTangoWorldConfig::valuesAttributePropertyName()
{
    return d->valueAttrPropName;
}

void CuTangoWorldConfig::setStateColorName(Tango::DevState s, const std::string &c)
{
    d->stateColors[s] = c;
}

int CuTangoWorldConfig::numStates() const
{
    return d->stateColors.size();
}

void CuTangoWorldConfig::setStateString(Tango::DevState s, const std::string &str)
{
    d->stateStrings[s] = str;
}

const std::string CuTangoWorldConfig::stateString(Tango::DevState s) const
{
    if(d->stateStrings.find(s) != d->stateStrings.end())
        return d->stateStrings[s];
    return "UNDEFINED STATE";
}

string CuTangoWorldConfig::successColor(bool success) const
{
    return d->successColors[success];
}

void CuTangoWorldConfig::setSuccessColor(bool success, const string &colorname)
{
    d->successColors[success] = colorname;
}

const std::string CuTangoWorldConfig::stateColorName(Tango::DevState s) const
{
    if(d->stateColors.find(s) != d->stateColors.end())
        return d->stateColors[s];
    return "gray";
}






