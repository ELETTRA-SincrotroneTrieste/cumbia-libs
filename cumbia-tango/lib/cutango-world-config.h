#ifndef QUTCONFIG_H
#define QUTCONFIG_H

#include <tango.h>
#include <string>
#include <vector>
#include <map>


class CuTangoWorldConfigPrivate;

/** \brief A class used to store configuration for Tango states color and name,
 *         Tango AttrQuality color and name and a color to associate to a successful
 *         operation.
 *
 * \section Default values
 *
 * \par State names association table
 *  [Tango::ON] =     "ON";
    [Tango::OFF] =    "OFF";
    [Tango::CLOSE] =  "CLOSED";
    [Tango::OPEN] =   "OPENED";
    [Tango::INSERT]     = "INSERT";
    [Tango::EXTRACT]    = "EXTRACT";
    [Tango::MOVING]     = "MOVING";
    [Tango::STANDBY]    = "STANDBY";
    [Tango::FAULT]      = "FAULT";
    [Tango::INIT]       = "INIT";
    [Tango::RUNNING]    = "RUNNING";
    [Tango::ALARM]      = "ALARM";
    [Tango::DISABLE]    = "DISABLE";
    [Tango::UNKNOWN]    = "UNKNOWN";

 * \par State color association table
 *
 * Several white versions are defined in order to easily map the color name
 * to a different QColor in graphical applications.
 *
 *
    [Tango::ON]          = "green";
    [Tango::OFF]         = "white";
    [Tango::CLOSE]       = "white1";
    [Tango::OPEN]        = "white2";
    [Tango::INSERT]      = "white3";
    [Tango::EXTRACT]     = "white4";
    [Tango::MOVING]      = "blue";
    [Tango::STANDBY]     = "yellow";
    [Tango::FAULT]       = "red";
    [Tango::INIT]        = "blue";
    [Tango::RUNNING]     = "green";
    [Tango::ALARM]       = "orange";
    [Tango::DISABLE]     = "gray";
    [Tango::UNKNOWN]     = "darkGray";
 *
 * \par Quality names
 *
    [Tango::ATTR_VALID] = "ATTR_VALID";
    [Tango::ATTR_INVALID] = "ATTR_INVALID";
    [Tango::ATTR_ALARM] = "ATTR_ALARM";
    [Tango::ATTR_CHANGING] = "ATTR_CHANGING";
    [Tango::ATTR_WARNING] = "ATTR_WARNING";
 *
 * \par Quality colors
 *
    [Tango::ATTR_VALID] = "white";
    [Tango::ATTR_INVALID] = "gray";
    [Tango::ATTR_ALARM] = "red";
    [Tango::ATTR_CHANGING] = "blue";
    [Tango::ATTR_WARNING] = "orange";
 *
 */
class CuTangoWorldConfig
{
public:

    CuTangoWorldConfig();

    virtual ~CuTangoWorldConfig();

    void setStateColors(const std::vector<std::string> &);

    void setStateColorName(Tango::DevState, const std::string&);

    const std::string stateColorName(Tango::DevState) const;
    int numStates() const;

    const std::string qualityColor(Tango::AttrQuality) const;


    void setQualityColor(Tango::AttrQuality, const std::string &);

    int qualityColorCount() const;

    void setStateString(Tango::DevState, const std::string &);
    const std::string	stateString(Tango::DevState) const;

    std::string successColor(bool success) const;

    void setSuccessColor(bool success, const std::string &colorname);

    void setQualityString(Tango::AttrQuality, std::string);
    std::string qualityString(Tango::AttrQuality) const;

    std::map<Tango::DevState, std::string> stateStrings();
    std::map<Tango::DevState, std::string>  stateColorNames();

    std::map<Tango::AttrQuality, std::string> qualityStrings();
    std::map<Tango::AttrQuality, std::string> qualityColorNames();



    /** \brief set the attribute property name for the QTangoAutoConfiguration "values" retrieval
         *
         * This is a <strong>work around</strong> to let QTangoAutoConfiguration provide the string list
         * associated to the attribute property <em>values</em>, even if the attribute property hasn't
         * got the <em>standard name <em>values</em></em>.
         */
    void setOverrideValuesAttributePropertyName(const std::string& name);

    std::string valuesAttributePropertyName();

protected:
    
private:

    CuTangoWorldConfigPrivate *d;
    
    void initStateColorsAndStrings();
    void initQualityColorsAndStrings();
    
};

#endif
