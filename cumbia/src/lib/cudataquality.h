#ifndef CUDATAQUALITY_H
#define CUDATAQUALITY_H

#include <string>

/**
 * @brief The CuDataQuality class maps quality descriptions that concern specific engines into an
 *        *engine independent data quality* information.
 *
 * Engines that provide a quality for their data will use this class to make it engine independent
 *
 * The set methods allow to OR multiple CuDataQuality::Type flags together in case quality
 * needs describe multiple results. So, if quality represents the result
 * of multiple readings, its type will have the CuDataQuality::Alarm flat set to true if one
 * of the results is in alarm.
 */
class CuDataQuality
{
public:
    enum Type { Undefined = 0, Invalid = 0x1, Valid=0x2, Changing=0x4, Warning=0x8, Alarm=0x10, MaxQualityType = 0x1000000 };

    CuDataQuality();

    CuDataQuality(Type t);

    CuDataQuality(int t);

    Type type() const;

    int toInt() const;

    void set(int t);

    void set(Type t);

    bool isSet(Type t) const;

    bool isInvalid() const;

    bool isUndefined() const;

    CuDataQuality& operator = (Type t);

    CuDataQuality& operator = (const CuDataQuality& other);

    std::string name() const;

    std::string color() const;

private:
    int m_type;
};

#endif // CUDATAQUALITY_H
