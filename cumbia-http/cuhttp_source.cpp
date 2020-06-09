#include "cuhttp_source.h"
#include "protocol/tango/cuhttptangosrc.h"
#include "protocol/tango/cuhttptangohelper.h"
#include <stdio.h>
#include <algorithm>
#include <regex>
#include <qustringlist.h>
#include <QRegularExpression>
#include <QtDebug>

CuHTTPSrc::CuHTTPSrc() {
}

CuHTTPSrc::~CuHTTPSrc() {
}

CuHTTPSrc::CuHTTPSrc(const string &s, const QList<CuHttpSrcHelper_I *> &srchs) {
    d.m_s = s;
    d.helper = m_find_helper(s, srchs);
}

CuHTTPSrc::CuHTTPSrc(const CuHTTPSrc &other) {
    this->d.m_s = other.d.m_s;
    this->d.helper = other.d.helper;
}

/*!
 * \brief returns true if the source is suitable to be monitored
 * \return true if the source is a reader that can be monitored over time or if no helpers
 *         are installed. false if an installed helper's CuHttpSrcHelper_I::can_monitor returns
 *         negative for the given source.
 *
 * \par Example
 * if CuHttpTangoSrcHelper is installed, canMonitor will return true if a source is an attribute or a command.
 * They are indeed suitable to monitor values over time. The method returns false if the Tango helper detects
 * a database data fetch related source, which shall be a one shot operation.
 *
 * \par Default behaviour
 * If no helpers are installed, it is assumed that a source *can monitor* and true is returned
 */
bool CuHTTPSrc::canMonitor() const {
    return d.helper ? ( d.helper->can_monitor(d.m_s) < 0 ? false : true ) : true;
}

/*!
 * \brief Returns the full name that was given to the constructor without the *cumbia domain
 *        engine* specification
 *
 * \return the source that was passed to the constructor
 */
string CuHTTPSrc::getName() const {
    return d.m_s;
}

string CuHTTPSrc::native_type() const {
    return d.helper ? d.helper->native_type() : std::string();
}

string CuHTTPSrc::prepare() const {
    return d.helper ? d.helper->prepare(d.m_s) : d.m_s;
}

string CuHTTPSrc::get(const char *what) const {
    return d.helper ? d.helper->get(d.m_s, what) : std::string();
}

CuHttpSrcHelper_I* CuHTTPSrc::m_find_helper(const std::string &src, const QList<CuHttpSrcHelper_I*>& helpers) {
    qDebug() << __PRETTY_FUNCTION__ << "finding helper for " << src.c_str() << "helpers size" << helpers.size();
    for(int i = 0; i < helpers.size(); i++)
        if(helpers[i]->is_valid(src)) return helpers[i];
    return nullptr;
}

/*! \brief matches last protocol specification found in the source name, matching the pattern
 *         <strong>protocol://</strong>
 *
 * \par example
 * In the source "https://pwma-dev.elettra.eu:10443/v1/cs/tango://ken.elettra.trieste.it:20000/test/device/1/double_scalar"
 * "tango://" is returned
 */
string CuHTTPSrc::getProtocol() const {
    std::regex base_regex("([a-zA-Z0-9_]+)://");
    string source = d.m_s;
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

CuHTTPSrc &CuHTTPSrc::operator=(const CuHTTPSrc &other) {
    if(this != &other) {
        d.m_s = other.d.m_s;
        this->d.helper = other.d.helper;
    }
    return *this;
}

/*!
 * \brief returns true if this source raw name equals other's
 * \param other another CuHTTPSrc
 * \return true if raw names are the same
 */
bool CuHTTPSrc::operator ==(const CuHTTPSrc &other) const {
    return d.m_s == other.d.m_s;
}

std::string CuHTTPSrc::toString() const
{
    char repr[512];
    snprintf(repr, 512, "HTTPSource [name:\"%s\"]",  d.m_s.c_str());
    return std::string(repr);
}
