#include "cuepics-world-config.h"

class CuEpicsWorldConfigPrivate
{
public:

    std::map<int, std::string> qualityStrings;
    std::map<int, std::string> qualityColors;

    std::map<bool, std::string> successColors;

    /* non standard device servers with non standard "values" attribute property name */
    std::string valueAttrPropName;
};

CuEpicsWorldConfig::CuEpicsWorldConfig()
{
    d = new CuEpicsWorldConfigPrivate;
    initQualityColorsAndStrings();

    d->successColors[true] = "dark_green";
    d->successColors[false] = "red";
    d->valueAttrPropName = "values";
}

CuEpicsWorldConfig::~CuEpicsWorldConfig()
{
    delete d;
}

void CuEpicsWorldConfig::initQualityColorsAndStrings()
{
    d->qualityStrings[0] = "ATTR_VALID";
    d->qualityStrings[1] = "ATTR_INVALID";
    d->qualityStrings[2] = "ATTR_ALARM";
    d->qualityStrings[3] = "ATTR_CHANGING";
    d->qualityStrings[4] = "ATTR_WARNING";
    d->qualityColors[5] = "white";
    d->qualityColors[6] = "gray";
    d->qualityColors[7] = "red";
    d->qualityColors[8] = "blue";
    d->qualityColors[9] = "orange";
}

const std::string CuEpicsWorldConfig::qualityColor(int q) const
{
    if(d->qualityColors.find(q) != d->qualityColors.end())
        return d->qualityColors[q];
    return "gray";
}

void CuEpicsWorldConfig::setQualityColor(int q, const std::string& c)
{
    d->qualityColors[q] = c;
}

int CuEpicsWorldConfig::qualityColorCount() const
{
    return d->qualityColors.size();
}

void CuEpicsWorldConfig::setQualityString(int q, std::string s)
{
    d->qualityStrings[q] = s;
}

std::string CuEpicsWorldConfig::qualityString(int q) const
{
    if(d->qualityStrings.find(q) != d->qualityStrings.end())
        return d->qualityStrings[q];
    return "UNDEFINED QUALITY";
}

std::map<int, std::string> CuEpicsWorldConfig::qualityStrings()
{
    return d->qualityStrings;
}

std::map<int, std::string> CuEpicsWorldConfig::qualityColorNames()
{
    return d->qualityColors;
}

void CuEpicsWorldConfig::setOverrideValuesAttributePropertyName(const std::string &name)
{
    d->valueAttrPropName = name;
}

std::string CuEpicsWorldConfig::valuesAttributePropertyName()
{
    return d->valueAttrPropName;
}

std::string CuEpicsWorldConfig::successColor(bool success) const
{
    return d->successColors[success];
}

void CuEpicsWorldConfig::setSuccessColor(bool success, const std::string &colorname)
{
    d->successColors[success] = colorname;
}






