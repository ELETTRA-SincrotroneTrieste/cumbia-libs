#include "botreadquality.h"
#include <tango.h>

BotReadQuality::BotReadQuality()
{
    m_q = Undefined;
}

BotReadQuality::BotReadQuality(const BotReadQuality &other)
{
    m_q = other.m_q;
}

/**
 * @brief BotReadQuality::fromTango build engine independent read quality from a Tango::AttrQuality quality
 * @param err if true, the quality is set to ReadError, if false, quality is determined by tango_quality
 * @param tango_quality maps Tango::AttrQuality into BotReadQuality::Quality
 */
void BotReadQuality::fromTango(bool err, int tango_quality)
{
    if(err)
        m_q = ReadError;
    else {
        switch(tango_quality) {
        case Tango::ATTR_ALARM:
            m_q = Alarm;
            break;
        case Tango::ATTR_VALID:
            m_q = Valid;
            break;
        case Tango::ATTR_INVALID:
            m_q = Invalid;
            break;
        case Tango::ATTR_WARNING:
            m_q = Warning;
            break;
        case Tango::ATTR_CHANGING:
            m_q = Changing;
            break;
        default:
            m_q = Undefined;
            break;
        }
    }
}

/**
 * @brief BotReadQuality::fromEval
 * @param err if true, quality is set to ReadError, if false, quality depends on eval being true or false
 * @param eval true (and err is false), quality is set to Alarm
 * @param eval false (and err is false), quality is set to Valid
 */
void BotReadQuality::fromEval(bool err, bool eval)
{
    if(err)
        m_q = ReadError;
    else if(eval) {
        m_q = Alarm;
    }
    else {
        m_q = Valid;
    }
}

BotReadQuality::Quality BotReadQuality::quality() const
{
    return m_q;
}

bool BotReadQuality::operator ==(const BotReadQuality &other) const
{
    return  this->m_q == other.m_q;
}

bool BotReadQuality::operator !=(const BotReadQuality &other) const
{
    return this->m_q != other.m_q;
}
