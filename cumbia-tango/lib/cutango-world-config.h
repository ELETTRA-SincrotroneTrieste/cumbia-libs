#ifndef QUTCONFIG_H
#define QUTCONFIG_H

#include <tango.h>
#include <string>
#include <vector>
#include <map>


class CuTangoWorldConfigPrivate;

/** \brief A class containing some configurations useful to
 *  several other objects.
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
