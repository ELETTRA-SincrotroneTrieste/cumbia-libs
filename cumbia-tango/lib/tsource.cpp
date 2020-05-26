#include "tsource.h"
#include <stdio.h>
#include <algorithm>
#include <regex>

// tghost:PORT regex ([A-Z-a-z0-9\-_\.\+~]+:\d+)
#define TGHOST_RE "([A-Z-a-z0-9\\-_\\.\\+~]+:\\d+)"

// tango device name regex
// used only if the source is not a command and the number of '/' is not 3
// otherwise string substr + find is enough
// ((?:tango://){0,1}(?:[A-Z-a-z0-9\-_\.\+~]+:\d*/){0,1}[A-Z-a-z0-9\-_\.\+~]+/[A-Z-a-z0-9\-_\.\+~]+/[A-Z-a-z0-9\-_\.\+~]+)
#define TG_DEV_RE "((?:tango://){0,1}(?:[A-Z-a-z0-9\\-_\\.\\+~]+:\\d*/){0,1}[A-Z-a-z0-9\\-_\\.\\+~]+/[A-Z-a-z0-9\\-_\\.\\+~]+/[A-Z-a-z0-9\\-_\\.\\+~]+)"

TSource::TSource() {
    m_ty = SrcInvalid;
}

TSource::TSource(const string s)
{
    m_s = s;
    m_ty = m_get_ty(s);

    cuprintf("TSource: src is %s database is \e[1;35m%s\e[0m \n", m_s.c_str(), getTangoHost(m_s).c_str());
    cuprintf("TSource: device is \e[1;32m%s\e[0m point is \e[1;36m%s\e[0m prop is \e[1;33m%s\e[0m search pattern: \e[0;33m%s\e[0m\n",
             getDeviceName().c_str(), getPoint().c_str(), getPropNam().c_str(), getSearchPattern().c_str());
    if(m_ty == SrcDbClassProp)
        cuprintf("TSource: class \e[1;34m%s\e[0m\n", getPropClassNam().c_str());
    cuprintf("TSource: detected type \e[1;32m%s\e[0m\n", getTypeName(m_ty));
}

TSource::TSource(const TSource &other)
{
    this->m_s = other.m_s;
    this->m_ty = other.m_ty;
}

TSource::Type TSource::m_get_ty(const std::string& src) const {
    // host regexp
    std::regex host_re("([A-Z-a-z0-9\\-_\\.\\+~]+:\\d+)");
    std::string s = rem_tghostproto(src);

    int sep = std::count(s.begin(), s.end(), '/');
    bool ewc = s.size() > 1 && s[s.size()-1] == '*'; // ends with wildcard
    bool ewsep = s.size() > 1 && s[s.size()-1] == '/'; // ends with slash
    bool has_ht = s.length() > 2 && s.find("#") != std::string::npos; // has #
    bool ewht = s.length() > 1 && s.find("#") == s.length() - 1;
    bool swht = s.length() > 1 && s[0] == '#'; // starts with hash tag: free prop
    size_t ai = s.find("->"); // arrow index
    bool ewa, hasa;
    ewa = s.size() > 2 && (ai == s.length() - 2); // ends with arrow do/fa/me->
    hasa = s.size() > 2 && ai != std::string::npos;
    Type t = SrcInvalid;
    if(swht) // free prop:  :MyFreeProp
        t= SrcDbFreeProp;
    else if(ewht && sep == 0) // class:
        t = SrcDbClassProps;
    else if(!ewht && has_ht && sep == 0) // class#prop
        t = SrcDbClassProp;
    else if(ewht && sep == 2)   //   a/tg/dev#  get device prop list
        t = SrcDbDevProps;
    else if(has_ht && sep == 2)  //  a/tg/dev#devprop
        t = SrcDbDevProp;
    else if(sep == 0 && ewc) // domai*
        t = SrcDbDoma;
    else if(sep == 1 && (ewsep || ewc)) // dom/  or  dom/fa*
        t = SrcDbFam;
    else if(sep == 2 && (ewsep || ewc)) // dom/fam/  or  dom/fam/me*
        t = SrcDbMem;
    else if(sep == 2 && ewa) // dom/fam/member->
        t = SrcDbCmds;
    else if(sep == 3 && hasa && !ewa && ewsep)  //   dom/fam/mem->cmd/  has arrow but not ends with arrow
        t = SrcDbGetCmdI;
    else if(sep == 3 && ewsep)  //   dom/fam/mem/
        t = SrcDbAtts;
    else if(sep == 4 && ewsep)  // dom/fam/mem/attr/
        t = SrcDbAProps;
    else if(sep == 3 && has_ht) //  a/tg/dev/attr#prop
        t = SrcDbAProp;
    else if(sep == 3)  // te/de/1/double_a
        t = SrcAttr;
    else if(sep == 2 && hasa) // te/de/1->GetV
        t = SrcCmd;

    return t;
}

string TSource::getDeviceName() const {
    string dev;
    /* attribute or command ? */
    size_t pos = m_s.rfind("->");
    if(pos == string::npos) {
        /* attribute */
        // remove host and tango://
        std::string s = rem_tghostproto(m_s);
        if(std::count(s.begin(), s.end(), '/') == 3) // a/tg/dev/attr: 3 `/' once removed tango://db:PORT/
            dev = m_s.substr(0, m_s.rfind('/'));
        else {
            // need regex
            std::regex dre(TG_DEV_RE);
            std::smatch sm;
            if(std::regex_search(m_s, sm, dre) && sm.size() == 2)
                dev = sm[1];
        }
    }
    else {
        dev = m_s.substr(0, pos);
    }
    return dev;
}

/*!
 * \brief Returns the attribute name if the type is SrcAttr, the command name if the type is SrcCmd,
 *        an empty string in all other cases
 */
string TSource::getPoint() const {
    string p;
    size_t pos = m_s.rfind("->");
    if(m_ty == SrcAttr) /* attribute */
        p = m_s.substr(m_s.rfind('/') + 1, m_s.find('(') - m_s.rfind('/') - 1); /* exclude args between parentheses */
    else if(m_ty == SrcCmd)
        p = m_s.substr(pos + 2, m_s.find('(') - pos - 2); /* exclude args */
    return p;
}

/*!
 * \brief Finds and returns the  comma separated arguments within parentheses, if present
 * \return vector of the detected arguments, as string
 */
std::vector<string> TSource::getArgs() const {
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

std::string TSource::getArgsString() const {
    std::string a;
    size_t pos = m_s.find('(');
    if(pos != string::npos)
        a = m_s.substr(pos + 1, m_s.rfind(')') - pos - 1);
    return a;
}

std::string TSource::getName() const {
    return m_s;
}

std::string TSource::getTangoHost(const std::string& src) const {
    std::regex host_re(TGHOST_RE);
    std::smatch sm;
    if(std::regex_search(src, sm, host_re) && sm.size() > 1)
        return sm[1];
    return std::string();
}

/*!
 * \brief Returns whatever follows the "#" character, if present
 * \return the subsctring of the source after the <em>#</em> character, if
 * present. It should be a property name.
 * An empty string is returned if no "#" character is found.
 *
 * \par Examples
 * - hokuto:20000/test/device/1/double_scalar#values (values attribute property)
 * - test/de/1#dprop  (dprop device property)
 * - TangoTest#version (version class property)
 *
 */
std::string TSource::getPropNam() const {
    std::string p;
    size_t i = m_s.find("#");
    if(i != std::string::npos)
        p = m_s.substr(i + 1);
    return p;
}

/*!
 * \brief If the source is a class property, get the class name
 * \return the class name found in the source
 */
string TSource::getPropClassNam() const {
    std::string p;
    if(m_ty == SrcDbClassProp) {
        std::string s = rem_tghostproto(m_s);
        size_t i = s.find("#");
        if(i != std::string::npos)
            p = s.substr(0, i);
    }
    return p;
}

/*!
 * \brief if the type is one of the database search methods, the search pattern
 *
 * \return
 */
string TSource::getSearchPattern() const {
    std::string s = rem_tghostproto(m_s);
    if(m_ty >= SrcDbDoma && m_ty <= SrcDbDevProps)
        return s;
    return std::string();
}

/*!
 * \brief returns true if the source represents an operation on the Tango Database
 */
bool TSource::isDbOp() const {
    return m_ty > SrcAttr && m_ty < SrcEndTypes;
}

TSource::Type TSource::getType() const {
    return m_ty;
}

TSource &TSource::operator=(const TSource &other) {
    if(this != &other) {
        m_s = other.m_s;
        m_ty = other.m_ty;
    }
    return *this;
}

bool TSource::operator ==(const TSource &other) const
{
    return m_s == other.m_s && m_ty == other.m_ty;
}

std::string TSource::toString() const
{
    char repr[512];
    snprintf(repr, 512, "TSource [%p] [name:\"%s\"] [device:\"%s\"] [point:\"%s\"] [type:%s] [args:\"%s\"]",
             this, m_s.c_str(), getDeviceName().c_str(), getPoint().c_str(), getTypeName(m_ty), getArgsString().c_str());
    return std::string(repr);
}

string TSource::remove_tgproto(const string &src) const {
    return src.size() > 0 && src.find("tango://") == 0 ? src.substr(strlen("tango://")) : src;
}

string TSource::remove_tghost(const string &src) const {
    std::regex host_re("([A-Z-a-z0-9\\-_\\.\\+~]+:\\d+[/#])");
    std::string s = std::regex_replace(src, host_re, "");
    return s;
}

string TSource::rem_tghostproto(const string &src) const
{
    std::string s = remove_tgproto(src);
    s = remove_tghost(s);
    return s;
}

const char *TSource::getTypeName(Type t) const {
    if(t < SrcEndTypes)
        return tynames[t];
    return tynames[0]; // "SrcInvalid"
}
