#ifndef QUTCONFIG_H
#define QUTCONFIG_H

#include <cadef.h>
#include <string>
#include <vector>
#include <map>


class CuEpicsWorldConfigPrivate;

/** \brief A class containing some configurations useful to
 *  several other objects.
 */
class CuEpicsWorldConfig
{
public:

    CuEpicsWorldConfig();

    virtual ~CuEpicsWorldConfig();

    const std::string qualityColor(int q) const;

    void setQualityColor(int, const std::string &);

    int qualityColorCount() const;

    std::string successColor(bool success) const;

    void setSuccessColor(bool success, const std::string &colorname);

    void setQualityString(int, std::string);
    std::string qualityString(int q) const;

    std::map<int, std::string> qualityStrings();
    std::map<int, std::string> qualityColorNames();



    /** \brief set the attribute property name for the QEpicsAutoConfiguration "values" retrieval
         *
         * This is a <strong>work around</strong> to let QEpicsAutoConfiguration provide the string list
         * associated to the attribute property <em>values</em>, even if the attribute property hasn't
         * got the <em>standard name <em>values</em></em>.
         */
    void setOverrideValuesAttributePropertyName(const std::string& name);

    std::string valuesAttributePropertyName();

protected:
    
private:

    CuEpicsWorldConfigPrivate *d;
    
    void initStateColorsAndStrings();
    void initQualityColorsAndStrings();
    
};

#endif
