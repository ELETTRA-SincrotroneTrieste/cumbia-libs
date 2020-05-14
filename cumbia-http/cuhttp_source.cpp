#include "cuhttp_source.h"
#include <stdio.h>
#include <algorithm>
#include <regex>

HTTPSource::HTTPSource() { }

HTTPSource::HTTPSource(const string& s) {
    m_s = s;
}

HTTPSource::HTTPSource(const HTTPSource &other) {
    this->m_s = other.m_s;
}

std::vector<string> HTTPSource::getArgs() const {
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

std::string HTTPSource::getArgsString() const {
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
 * \return the source that was passed to the constructor
 */
string HTTPSource::getName() const {
    return m_s;
}

/*!
 * \brief Returns the source name as given to the constructor without arguments or placeholders
 *
 * \return source name without arguments or placeholders
 *
 * \par Example
 * \code
 * HTTPSource s("myhost:20000/ab/cd/ef/hi(200)");
 * HTTPSource s2("myhost:20000/ab/cd/ef/hello(&hello_spinbox)");
 * s.getName(); // "myhost:20000/ab/cd/ef/hi(200)"
 * s.getNameNoArgs(); // "myhost:20000/ab/cd/ef/hi"
 * s2.getNameNoArgs(); // "myhost:20000/ab/cd/ef/hello"
 * \endcode
 */
string HTTPSource::getNameNoArgs() const {
    return m_s.substr(0, m_s.find("("));
}

/*! \brief matches last protocol specification found in the source name, matching the pattern
 *         <strong>protocol://</strong>
 *
 * \par example
 * In the source "https://pwma-dev.elettra.eu:10443/v1/cs/tango://ken.elettra.trieste.it:20000/test/device/1/double_scalar"
 * "tango://" is returned
 */
string HTTPSource::getProtocol() const {
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

HTTPSource &HTTPSource::operator=(const HTTPSource &other) {
    if(this != &other)
        m_s = other.m_s;
    return *this;
}

bool HTTPSource::operator ==(const HTTPSource &other) const {
    return m_s == other.m_s;
}

std::string HTTPSource::toString() const
{
    char repr[512];
    snprintf(repr, 512, "HTTPSource [name:\"%s\"] [args:\"%s\"]",  m_s.c_str(), getArgsString().c_str());
    return std::string(repr);
}
