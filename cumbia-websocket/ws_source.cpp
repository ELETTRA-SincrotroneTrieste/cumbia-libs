#include "ws_source.h"
#include <stdio.h>
#include <algorithm>
#include <regex>

WSSource::WSSource()
{

}

WSSource::WSSource(const string& s)
{
    m_s = s;
}

WSSource::WSSource(const WSSource &other)
{
    this->m_s = other.m_s;
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

/*!
 * \brief Returns the full name that was given to the constructor without the *cumbia domain
 *        engine* specification
 *
 * \return the source that was passed to the constructor, without the *cumbia domain
 *         engine* specification, if specified (either ws:// or wss://)
 *
 * \par example
 * \code
 * WSSource s("wss://tango://test/device/1/double_scalar");
 * printf("name: %s\n", s.getName().c_str());
 * //
 * // output
 * // name: tango://test/device/1/double_scalar
 * //
 * \endcode
 *
 */
string WSSource::getName() const
{
    std::regex re("ws[s]{0,1}://");
    std::smatch match;
    if(std::regex_search(m_s, match, re))
        return m_s.substr(match.position() + match.length());
    return m_s;
}

/*!
 * \brief WSSource::getFullName returns the full name that was given to the constructor
 * \return the full name that was given to the constructor, including the *cumbia domain
 *         engine* specification, if provided (either ws:// or wss://)
 */
string WSSource::getFullName() const {
    return m_s;
}

/*! \brief matches last protocol specification found in the source name, matching the pattern
 *         <strong>protocol://</strong>
 *
 * \par example
 * In the source "https://pwma-dev.elettra.eu:10443/v1/cs/tango://ken.elettra.trieste.it:20000/test/device/1/double_scalar"
 * "tango://" is returned
 */
string WSSource::getProtocol() const {
    std::regex base_regex("([a-zA-Z0-9_]+)://");
    string source = m_s;
    // default constructor = end-of-sequence:
    std::vector<std::string> matches;
    std::regex_token_iterator<std::string::iterator> rend;
    // parameter 1: match 1st submatch (in parentheses)
    std::regex_token_iterator<std::string::iterator> a ( source.begin(), source.end(), base_regex, 1 );
    while (a!=rend) {
        matches.push_back(*a);
        a++;
    }
    if(matches.size() > 0)
        return matches.at(matches.size() - 1);
    return "";
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
    snprintf(repr, 512, "WSSource [name:\"%s\"] [args:\"%s\"]",  m_s.c_str(), getArgsString().c_str());
    return std::string(repr);
}
