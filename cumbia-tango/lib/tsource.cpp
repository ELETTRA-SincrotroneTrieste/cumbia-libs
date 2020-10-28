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
    const std::vector<string> props = getPropNames();
    bool hasprops = props.size() > 0; // has {arg1,arg2,...}
    bool arg_wildcard = props.size() == 1 && props[0] == "*";
    bool swht = s.length() > 1 && s[0] == '#'; // starts with hash tag: free prop
    size_t ai = s.find("->"); // arrow index
    bool ewa, hasa;
    ewa = s.size() > 2 && (ai == s.length() - 2); // ends with arrow do/fa/me->
    hasa = s.size() > 2 && ai != std::string::npos;
    Type t = SrcInvalid;
    if(swht && std::count(s.begin(), s.end(), '#') == 2) // free prop:  #MyObj#MyFreeProp
        t= SrcDbFreeProp;
    else if(arg_wildcard && std::count(s.begin(), s.end(), '*') > 1) // at least 2 (one shall be within (*) )
        t = SrcExportedDevs;
    else if(arg_wildcard && sep == 0) // class(*))
        t = SrcDbClassProps;
    else if(hasprops && sep == 0) // class{prop1,prop2,..}
        t = SrcDbClassProp;
    else if(arg_wildcard && sep == 2)   //   a/tg/dev(*)  get device prop list
        t = SrcDbDevProps;
    else if(hasprops && sep == 2 && !hasa)  //  a/tg/dev(devprop1,devprop2,...)
        t = SrcDbDevProp;
    else if(s.size() == 0 || s == "*") // domai*
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
        t = SrcDbAttInfo;
    else if(sep == 3 && hasprops) //  a/tg/dev/attr(prop1,prop2,...)
        t = SrcDbAProp;
    else if(sep == 3)  // te/de/1/double_a
        t = SrcAttr;
    else if(sep == 2 && hasa) // te/de/1->GetV
        t = SrcCmd;

//    printf("TSource m_getTy: s is %s type is %d\n", s.c_str(), t);
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

string TSource::getDeviceNameOnly() const {
    return rem_tghostproto(getDeviceName());
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
    else if(m_ty == SrcDbGetCmdI)
        p = m_s.substr(m_s.find("->") + 2, m_s.rfind('/') - m_s.find("->")  -2); // src is tango://test/device/1->get/
    else if(m_ty == SrcDbAttInfo) { // src is tango://test/device/1/get/
        // remove last /
        std::string s = m_s.substr(0, m_s.rfind('/'));
        p = s.substr(s.rfind('/') + 1);
    }
    else if(m_ty == SrcDbAProp) // src is tango://hokuto:20000/test/device/1/double_scalar(p1,p2,..)
        p = m_s.substr(m_s.rfind('/') + 1, m_s.rfind("(")- m_s.rfind('/') -1 );
    return p;
}

/*!
 * \brief Finds and returns the  comma separated arguments within parentheses, if present
 * \param curly_b_delim if true, search args between {}. Default: false
 * \return vector of the detected arguments, as string
 */
std::vector<string> TSource::getArgs() const {
    std::string a;
    std::string delim = ",";
    std::regex re(delim);
    std::vector<std::string> ret;
    std::string s(m_s);
//    s.erase(std::remove(s.begin() + s.find('('), s.begin() + s.find(')') + 1, ' '), s.end()); // remove spaces
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

std::vector<std::string> TSource::getPropNames() const {
    return getArgs();
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

std::string TSource::getTangoHost() const {
    std::regex host_re(TGHOST_RE);
    std::smatch sm;
    if(std::regex_search(m_s, sm, host_re) && sm.size() > 1)
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
 * - #ObjectName#FreePropName (version free property)
 *
 * @see getFreePropObj
 *
 */
std::string TSource::getFreePropNam() const {
    std::string p;
    size_t i = m_s.rfind("#");
    if(i != std::string::npos)
        p = m_s.substr(i + 1);
    return p;
}

/*!
 * \brief get the object name the free property refers to
 * \return the object name or an empty string
 *
 * \par Syntax
 * - #MyObject#MyProperty getFreePropObj returns MyObject while getPropNam returns MyProperty
 * \par Example
 * - #Sequencer#TestList getFreePropObj returns Sequencer while getPropNam returns TestList
 */
string TSource::getFreePropObj() const {
    std::regex re("#(.*)#");  // #(.*)#
    std::smatch sm;
    if(std::regex_search(m_s, sm, re) && sm.size() > 1)
        return sm[1];
    return std::string();
}

/*!
 * \brief If the source is a class property, get the class name
 * \return the class name found in the source
 */
string TSource::getPropClassNam() const {
    std::string p;
    if(m_ty == SrcDbClassProp || m_ty == SrcDbClassProps) {
        std::string s = rem_tghostproto(m_s);
        size_t i = s.find("(");
        if(i != std::string::npos)
            p = s.substr(0, i);
    }
    return p;
}

string TSource::getExportedDevSearchPattern() const {
    std::regex re("(.*)\\(\\*\\)");  // hokuto:20000/test/*/1(*)
    const std::string &s = rem_tghostproto(m_s); // test/*/1(*)
    std::smatch sm;
    if(std::regex_search(s, sm, re) && sm.size() > 1)
        return sm[1];
    return std::string();
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
