#include "ws_source.h"
#include <stdio.h>
#include <algorithm>
#include <regex>

WSSource::WSSource()
{

}

WSSource::WSSource(const string s)
{
    m_s = s;
}

WSSource::WSSource(const WSSource &other)
{
    this->m_s = other.m_s;
}

string WSSource::getDeviceName() const
{
    string dev;
    /* attribute or command ? */
    size_t pos = m_s.rfind("->");
    if(pos == string::npos)
    {
        /* attribute */
        dev = m_s.substr(0, m_s.rfind('/'));
    }
    else
    {
        dev = m_s.substr(0, pos);
    }
    return dev;
}

string WSSource::getPoint() const
{
    string p;
    size_t pos = m_s.rfind("->");
    if(pos == string::npos) /* attribute */
        p = m_s.substr(m_s.rfind('/') + 1, m_s.find('(') - m_s.rfind('/') - 1); /* exclude args between parentheses */
    else
        p = m_s.substr(pos + 2, m_s.find('(') - pos - 2); /* exclude args */
    return p;
}

std::vector<string> WSSource::getArgs() const
{
    std::string a = getArgsString();
    std::string delim = ",";
    std::regex re(delim);
    std::vector<std::string> ret;
    size_t pos = m_s.find('(');
    if(pos != string::npos)
        a = m_s.substr(pos + 1, m_s.rfind(')') - pos - 1);
    std::sregex_token_iterator iter(a.begin(), a.end(), re, -1);
    std::sregex_token_iterator end;
    for ( ; iter != end; ++iter)
        if((*iter).length() > 0)
            ret.push_back((*iter));
    return ret;
}

std::string WSSource::getArgsString() const
{
    std::string a;
    size_t pos = m_s.find('(');
    if(pos != string::npos)
        a = m_s.substr(pos + 1, m_s.rfind(')') - pos - 1);
    return a;
}

string WSSource::getName() const
{
    return m_s;
}

WSSource::Type WSSource::getType() const
{
    if(m_s.rfind("->") == string::npos)
        return Attr;
    return Cmd;
}

WSSource &WSSource::operator=(const WSSource &other)
{
    if(this != &other)
        m_s = other.m_s;
    return *this;
}

bool WSSource::operator ==(const WSSource &other) const
{
    return m_s == other.m_s;
}

std::string WSSource::toString() const
{
    char repr[512];
    char type[8];
    getType() == Attr ? snprintf(type, 8, "attr") :  snprintf(type, 8, "cmd");
    snprintf(repr, 512, "WSSource [%p] [name:\"%s\"] [device:\"%s\"] [point:\"%s\"] [type:%s] [args:\"%s\"]",
             this, m_s.c_str(), getDeviceName().c_str(), getPoint().c_str(), type, getArgsString().c_str());
    return std::string(repr);
}
