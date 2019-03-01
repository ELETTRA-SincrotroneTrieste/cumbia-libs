#include "cudataquality.h"

/**
 * @brief CuDataQuality::CuDataQuality builds a Valid CuDataQuality (Type Invalid: 0)
 */
CuDataQuality::CuDataQuality()
{
    m_type = Valid;
}

/**
 * @brief CuDataQuality::CuDataQuality builds a CuDataQuality with the given quality
 * @param t CuDataQuality::Type quality
 */
CuDataQuality::CuDataQuality(int t) {
    m_type = static_cast<Type>(t);
}

/**
 * @brief CuDataQuality::CuDataQuality same as the other one parameter constructor, but
 *        requires CuDataQuality::Type input.
 * @param t CuDataQuality::Type, the quality that initialises this object
 */
CuDataQuality::CuDataQuality(CuDataQuality::Type t)
{
    m_type = t;
}

/**
 * @brief CuDataQuality::set sets the given flag on the data quality type
 * @param t CuDataQuality::Type as integer to be ored with the current type
 */
void CuDataQuality::set(int t)
{
    m_type |= static_cast<CuDataQuality::Type>(t);
}

/**
 * @brief CuDataQuality::set sets the given flag on the data quality type
 * @param t CuDataQuality::Type, will be ORed with the current type
 */
void CuDataQuality::set(CuDataQuality::Type t)
{
    m_type |= t;
}

/**
 * @brief CuDataQuality::isSet returns true if the given quality type is set, false otherwise
 * @param t CuDataQuality::Type
 * @return true if t is set, false otherwise
 */
bool CuDataQuality::isSet(CuDataQuality::Type t) const
{
    return m_type & t;
}

/**
 * @brief CuDataQuality::isInvalid returns true if the CuDataQuality::Invalid is set
 * @return true if CuDataQuality::Invalid is set, false otherwise
 */
bool CuDataQuality::isInvalid() const
{
    return m_type & Invalid;
}

/**
 * @brief CuDataQuality::isUndefined returns true if the current quality is undefined
 * @return returns true if the current quality is undefined
 *
 * \par Note
 * If the empty constructor for CuDataQuality is used, a Valid quality is set by default.
 * You must explicitly set the quality to Undefined to have and *undefined* data quality.
 */
bool CuDataQuality::isUndefined() const
{
    return m_type == 0;
}

CuDataQuality &CuDataQuality::operator=(CuDataQuality::Type t)
{
    this->m_type = t;
    return *this;
}

CuDataQuality &CuDataQuality::operator =(const CuDataQuality &other)
{
   m_type = other.m_type;
   return *this;
}

bool CuDataQuality::operator==(const CuDataQuality &other) const
{
    return  m_type == other.m_type;
}

bool CuDataQuality::operator !=(const CuDataQuality &other) const
{
    return  m_type != other.m_type;
}

std::string CuDataQuality::name() const
{
    if(m_type & Invalid)
        return "invalid";
    if(m_type & CuDataQuality::Alarm)
        return std::string("alarm");
    if(m_type & CuDataQuality::Warning)
        return std::string("warning");
    if(m_type & CuDataQuality::Changing)
        return std::string("changing");
    if(m_type & Valid)
        return std::string("valid");
    if(m_type == 0)
        return "undefined";
    return std::string("invalid");
}

// "white", "gray", "red", "blue", "orange"
std::string CuDataQuality::color() const
{
    if(m_type & CuDataQuality::Invalid)
        return std::string("gray");
    if(m_type & CuDataQuality::Alarm)
        return std::string("red");
    if(m_type & CuDataQuality::Warning)
        return std::string("orange");
    if(m_type & CuDataQuality::Changing)
        return std::string("blue");
    if(m_type & Valid)
        return std::string("white");
    return std::string("gray");
}

CuDataQuality::Type CuDataQuality::type() const
{
    return static_cast<CuDataQuality::Type>(m_type);
}

int CuDataQuality::toInt() const
{
    return m_type;
}

