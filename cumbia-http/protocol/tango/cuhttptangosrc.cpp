#include "cuhttptangosrc.h"

#include "cuhttptangosrc.h"
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


std::regex& regexps::get_dev_re() {
    if(!dre) {
        dre = true;
        printf("regexps::get_dev_re: compiling dev regex\n");
        dev_re = std::regex(TG_DEV_RE);
    }
    return dev_re;
}

std::regex& regexps::get_host_re() {
    if(!hre) {
        hre = true;
        printf("regexps::get_host_re: compiling host regex\n");
        host_re = std::regex(TGHOST_RE);
    }
    return host_re;
}

regex &regexps::get_freeprop_re() {
    if(!fre) {
        fre = true;
        printf("regexps::get_freeprop_re: compiling free prop regex\n");
        freeprop_re = std::regex("#(.*)#");
    }
    return freeprop_re;
}

regex &regexps::get_args_re() {
    if(!are) {
        are = true;   // \(\[\s*(.*)\s*\]\s*.*\)
        printf("regexps::get_args_re: compiling args regex\n");
        args_re = std::regex("\\(\\[\\s*(.*)\\s*\\]\\s*.*\\)");
    }
    return args_re;
}

regex &regexps::get_separ_re()
{
    if(!sre) {
        sre = true;
        printf("regexps::get_separ_re: compiling separator regex\n");
        separ_re = std::regex("sep\\((.*)\\)");
    }
    return separ_re;
}

static regexps _regexps;

CuHttpTangoSrc::CuHttpTangoSrc() {
    m_ty = SrcInvalid;
}

CuHttpTangoSrc::CuHttpTangoSrc(const string s)
{
    m_s = s;
    m_ty = m_get_ty(s);
}

CuHttpTangoSrc::CuHttpTangoSrc(const CuHttpTangoSrc &other)
{
    this->m_s = other.m_s;
    this->m_ty = other.m_ty;
}

CuHttpTangoSrc::Type CuHttpTangoSrc::m_get_ty(const std::string& src) const {
    // host regexp
    // remove host and http://
    std::string s = rem_httpproto(m_s);
    s = rem_tghost(s);
    s = rem_args(s); // remove arguments between '()'
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
    else if(arg_wildcard && sep == 0) // class(*))
        t = SrcDbClassProps;
    else if(hasprops && sep == 0) // class{prop1,prop2,..}
        t = SrcDbClassProp;
    else if(arg_wildcard && sep == 2)   //   a/tg/dev(*)  get device prop list
        t = SrcDbDevProps;
    else if(hasprops && sep == 2 && !hasa)  //  a/tg/dev(devprop1,devprop2,...)
        t = SrcDbDevProp;
    else if( s == "/" || s == "/*") // domai*
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

    return t;
}

string CuHttpTangoSrc::getDeviceName() const {
    string dev;
    /* attribute or command ? */
    size_t pos = m_s.rfind("->");
    if(pos == string::npos) {
        /* attribute */
        // remove host and http://
        std::string s = rem_httpproto(m_s);
        s = rem_tghost(s);
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

string CuHttpTangoSrc::getDeviceNameOnly() const {
    std::string s = getDeviceName();
    s = rem_httpproto(s);
    s = rem_tghost(s);
    return s;
}

/*!
 * \brief Returns the attribute name if the type is SrcAttr, the command name if the type is SrcCmd,
 *        an empty string in all other cases
 */
string CuHttpTangoSrc::getPoint() const {
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
std::vector<string> CuHttpTangoSrc::getArgs() const {
    std::string a;
    std::string delim;
    std::vector<std::string> ret;
    std::string s(m_s);
    size_t arg_start = 0, arg_end = 0;
    const std::string& arg_ops = getArgOptions(&arg_start, &arg_end);
    //    s.erase(std::remove(s.begin() + s.find('('), s.begin() + s.find(')') + 1, ' '), s.end()); // remove spaces
    // take an argument delimited by "" as a single parameter
    size_t pos = m_s.find("(\"");
    if(pos != string::npos) {
        a = m_s.substr(pos + 2, m_s.rfind("\")") - pos - 2);
        ret.push_back(a);
    }
    else {
        pos = m_s.find('(');
        if(pos != string::npos) {
            a = m_s.substr(pos + 1, m_s.rfind(')') - pos - 1);
            if(a.length() > 0) {
                delim = arg_end > 0 ? m_get_args_delim(arg_ops) : ",";
                if(arg_end > 0) // recalculate a as substr from arg_end + 1
                    a = m_s.substr(arg_end + 1);
                std::regex re(delim);
                std::sregex_token_iterator iter(a.begin(), a.end(), re, -1);
                std::sregex_token_iterator end;
                for ( ; iter != end; ++iter)
                    if((*iter).length() > 0)
                        ret.push_back((*iter));
            }
        }
    }
    return ret;
}

std::vector<std::string> CuHttpTangoSrc::getPropNames() const {
    return getArgs();
}

std::string CuHttpTangoSrc::getArgsString() const {
    std::string a;
    size_t pos = m_s.find('(');
    if(pos != string::npos)
        a = m_s.substr(pos + 1, m_s.rfind(')') - pos - 1);
    return a;
}

std::string CuHttpTangoSrc::getName() const {
    return m_s;
}

std::string CuHttpTangoSrc::getTangoHost() const {
    std::smatch sm;
    if(std::regex_search(m_s, sm, _regexps.get_host_re()) && sm.size() > 1)
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
std::string CuHttpTangoSrc::getFreePropNam() const {
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
string CuHttpTangoSrc::getFreePropObj() const {
    std::smatch sm;
    if(std::regex_search(m_s, sm, _regexps.get_freeprop_re()) && sm.size() > 1)
        return sm[1];
    return std::string();
}

/*!
 * \brief If the source is a class property, get the class name
 * \return the class name found in the source
 */
string CuHttpTangoSrc::getPropClassNam() const {
    std::string p;
    if(m_ty == SrcDbClassProp || m_ty == SrcDbClassProps) {
        std::string s = rem_httpproto(m_s);
        s = rem_tghost(s);
        size_t i = s.find("(");
        if(i != std::string::npos)
            p = s.substr(0, i);
    }
    return p;
}

/*!
 * \since 1.5.2
 * \brief some keyword:value fields can be used at the beginning of the argument
 *        section to customize the interpretation of the arguments
 *
 *        The keyword:value list shall be enclosed between square brackets at the
 *        beginning of the arguments section
 *
 * \par  Example
 *       test/device/1/double_spectrum([sep(;)]10;20;30)
 *
 * \return a string with the options
 */
std::string CuHttpTangoSrc::getArgOptions(size_t *pos_start, size_t *pos_end) const {
    // capture special directives to interpret args
    // example a/b/c/d([sep(;)]arg1;arg2) sep: args separator
    const std::string &s = m_s;
    std::smatch sm;
    bool found = std::regex_search(s, sm, _regexps.get_args_re());
    if(found) {
        *pos_start = sm.position(1);
        *pos_end = *pos_start + sm.length(1);
    }
    return found && sm.size() == 2 ? sm[1] : std::string();
}

/*!
 * \brief if the type is one of the database search methods, the search pattern
 *
 * \return
 */
string CuHttpTangoSrc::getSearchPattern() const {
    std::string s = rem_httpproto(m_s);
    s = rem_tghost(s);
    if(m_ty >= SrcDbDoma && m_ty <= SrcDbDevProps)
        return s;
    return std::string();
}

/*!
 * \brief returns true if the source represents an operation on the Tango Database
 */
bool CuHttpTangoSrc::isDbOp() const {
    return m_ty > SrcAttr && m_ty < SrcEndTypes;
}

CuHttpTangoSrc::Type CuHttpTangoSrc::getType() const {
    return m_ty;
}

CuHttpTangoSrc &CuHttpTangoSrc::operator=(const CuHttpTangoSrc &other) {
    if(this != &other) {
        m_s = other.m_s;
        m_ty = other.m_ty;
    }
    return *this;
}

bool CuHttpTangoSrc::operator ==(const CuHttpTangoSrc &other) const
{
    return m_s == other.m_s && m_ty == other.m_ty;
}

bool CuHttpTangoSrc::isValid() const {
    return m_s.size() > 0;
}

std::string CuHttpTangoSrc::toString() const
{
    char repr[512];
    snprintf(repr, 512, "CuHttpTangoSrc [%p] [name:\"%s\"] [device:\"%s\"] [point:\"%s\"] [type:%s] [args:\"%s\"]",
             this, m_s.c_str(), getDeviceName().c_str(), getPoint().c_str(), getTypeName(m_ty), getArgsString().c_str());
    return std::string(repr);
}

string CuHttpTangoSrc::rem_httpproto(const string &src) const {
    return src.size() > 0 && src.find("http://") == 0 ? src.substr(strlen("http://")) : src;
}

string CuHttpTangoSrc::rem_tghost(const string &src) const {
    std::string s = std::regex_replace(src, _regexps.get_host_re(), "");
    return s;
}

std::string CuHttpTangoSrc::rem_args(const std::string& src) const {
    // capture everything within (\(.*\)), not minimal
    return std::regex_replace(src, _regexps.get_args_re(), "");
}

const char *CuHttpTangoSrc::getTypeName(Type t) const {
    if(t < SrcEndTypes)
        return tynames[t];
    return tynames[0]; // "SrcInvalid"
}

std::string CuHttpTangoSrc::m_get_args_delim(const string &arg_options) const
{
    // find a custom separator, if specified at the beginning of the args section
    //  sep\((.*)\)
    // example: a/b/c-D([sep(:)]arg1:arg2:arg3)
    std::smatch sm;
    return std::regex_search(arg_options, sm, _regexps.get_separ_re()) && sm.size() == 2 ? sm[1] : std::string(",");
}
