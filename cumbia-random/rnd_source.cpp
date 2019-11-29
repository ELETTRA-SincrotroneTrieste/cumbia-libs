#include "rnd_source.h"
#include <stdio.h>
#include <algorithm>
#include <regex>

RNDSource::RNDSource()
{

}

RNDSource::RNDSource(const string s)
{
    m_s = s;
}

RNDSource::RNDSource(const RNDSource &other)
{
    this->m_s = other.m_s;
}

string RNDSource::getName() const
{
    return m_s;
}

RNDSource &RNDSource::operator=(const RNDSource &other)
{
    if(this != &other)
        m_s = other.m_s;
    return *this;
}

bool RNDSource::operator ==(const RNDSource &other) const
{
    return m_s == other.m_s;
}

