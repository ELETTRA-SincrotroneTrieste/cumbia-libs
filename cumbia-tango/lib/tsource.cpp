#include "tsource.h"
#include <stdio.h>
#include <algorithm>
#include <regex>

/*
    std::regex dev_re(TG_DEV_RE);
    std::regex host_re(TGHOST_RE);
    std::regex freeprop_re("#(.*)#");  // #(.*)#
    std::regex argopts_re("\\(\\[\\s*(.*)\\s*\\]\\s*.*\\)");  // \(\[\s*(.*)\s*\]\s*.*\)
    std::regex args_re("(\\(.*\\))");
    std::regex separ_re("sep\\((.*)\\)");
*/

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
        args_re =std::regex("\\(\\[\\s*(.*)\\s*\\]\\s*.*\\)");
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

static regexps tsrc_regexps;

class TSourcePrivate {
public:
    string m_s;
    TSource::Type m_ty;
};

TSource::TSource() {
    d = new TSourcePrivate;
    d->m_ty = SrcInvalid;
}

TSource::TSource(const string s)
{
    d = new TSourcePrivate;
    d->m_s = s;
    d->m_ty = m_get_ty(s);
}

TSource::TSource(const TSource &other) {
    d = new TSourcePrivate;
    this->d->m_s = other.d->m_s;
    this->d->m_ty = other.d->m_ty;
}

TSource::~TSource() {
    delete d;
}

TSource::Type TSource::m_get_ty(const std::string& src) const {
    // host regexp

    std::string s = rem_tghostproto(src);
    s = rem_args(s); // remove arguments between from s
    const std::vector<string> props = getPropNames();
    bool hasprops = props.size() > 0; // has {arg1,arg2,...}
    int sep = std::count(s.begin(), s.end(), '/');
    bool ewc = s.size() > 1 && s[s.size()-1] == '*'; // ends with wildcard
    bool ewsep = s.size() > 1 && s[s.size()-1] == '/'; // ends with slash
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
    return t;
}

string TSource::getDeviceName() const {
    string dev;
    /* attribute or command ? */
    size_t pos = d->m_s.rfind("->");
    if(pos == string::npos) {
        /* attribute */
        // remove host and tango://
        std::string s = rem_tghostproto(d->m_s);
        if(std::count(s.begin(), s.end(), '/') == 3) // a/tg/dev/attr: 3 `/' once removed tango://db:PORT/
            dev = d->m_s.substr(0, d->m_s.rfind('/'));
        else {
            // need regex
            std::smatch sm;
            if(std::regex_search(d->m_s, sm, tsrc_regexps.get_dev_re()) && sm.size() == 2)
                dev = sm[1];
        }
    }
    else {
        dev = d->m_s.substr(0, pos);
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
    size_t pos = d->m_s.rfind("->");
    if(d->m_ty == SrcAttr) /* attribute */
        p = d->m_s.substr(d->m_s.rfind('/') + 1, d->m_s.find('(') - d->m_s.rfind('/') - 1); /* exclude args between parentheses */
    else if(d->m_ty == SrcCmd)
        p = d->m_s.substr(pos + 2, d->m_s.find('(') - pos - 2); /* exclude args */
    else if(d->m_ty == SrcDbGetCmdI)
        p = d->m_s.substr(d->m_s.find("->") + 2, d->m_s.rfind('/') - d->m_s.find("->")  -2); // src is tango://test/device/1->get/
    else if(d->m_ty == SrcDbAttInfo) { // src is tango://test/device/1/get/
        // remove last /
        std::string s = d->m_s.substr(0, d->m_s.rfind('/'));
        p = s.substr(s.rfind('/') + 1);
    }
    else if(d->m_ty == SrcDbAProp) // src is tango://hokuto:20000/test/device/1/double_scalar(p1,p2,..)
        p = d->m_s.substr(d->m_s.rfind('/') + 1, d->m_s.rfind("(")- d->m_s.rfind('/') -1 );
    return p;
}

/*!
 * \brief Finds and returns the comma separated arguments within parentheses, if present
 *
 *  List of options to individuate and interpret arguments can be specified between square
 *  brackets immediately after the opening parenthesis in the source string:
 *  - a/b/c->Get([sep(;)]arg1, arg2, arg3)
 * \return vector of the detected arguments, as string
 */
std::vector<string> TSource::getArgs() const {
    std::string a;
    std::string delim;
    std::vector<std::string> ret;
    std::string s(d->m_s);
    size_t arg_start = 0, arg_end = 0;
    const std::string& arg_ops = s.find('[') != std::string::npos ? getArgOptions(&arg_start, &arg_end) : std::string();
    //    s.erase(std::remove(s.begin() + s.find('('), s.begin() + s.find(')') + 1, ' '), s.end()); // remove spaces
    // take an argument delimited by "" as a single parameter
    size_t pos = d->m_s.find("(\"");
    if(pos != string::npos) {
        a = d->m_s.substr(pos + 2, d->m_s.rfind("\")") - pos - 2);
        ret.push_back(a);
    }
    else {
        pos = d->m_s.find('(');
        if(pos != string::npos) {
            a = d->m_s.substr(pos + 1, d->m_s.rfind(')') - pos - 1);
            if(a.length() > 0) {
                delim = arg_end > 0 ? m_get_args_delim(arg_ops) : ",";
                if(arg_end > 0) // recalculate a as substr from arg_end + 1
                    a = d->m_s.substr(arg_end + 1);
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

std::vector<std::string> TSource::getPropNames() const {
    return getArgs();
}

std::string TSource::getArgsString() const {
    std::string a;
    size_t pos = d->m_s.find('(');
    if(pos != std::string::npos)
        a = d->m_s.substr(pos + 1, d->m_s.rfind(')') - pos - 1);
    return a;
}

std::string TSource::getName() const {
    return d->m_s;
}

std::string TSource::getTangoHost() const {
    std::smatch sm;
    int pos = d->m_s.find(':'); // use regex only if :N is found (: plus digit, like tom:20000)
    if(pos != std::string::npos && pos > 0 && d->m_s.length() > pos + 1 && std::isdigit(d->m_s[pos+1]) &&
        std::regex_search(d->m_s, sm, tsrc_regexps.get_host_re()) && sm.size() > 1)
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
    size_t i = d->m_s.rfind("#");
    if(i != std::string::npos)
        p = d->m_s.substr(i + 1);
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
    std::smatch sm; // std::regex_search only if d->ms starts with '#'
    if(d->m_s.length() > 0 && d->m_s.find('#') != std::string::npos && std::regex_search(d->m_s, sm, tsrc_regexps.get_freeprop_re()) && sm.size() > 1)
        return sm[1];
    return std::string();
}

/*!
 * \brief If the source is a class property, get the class name
 * \return the class name found in the source
 */
string TSource::getPropClassNam() const {
    std::string p;
    if(d->m_ty == SrcDbClassProp || d->m_ty == SrcDbClassProps) {
        std::string s = rem_tghostproto(d->m_s);
        size_t i = s.find("(");
        if(i != std::string::npos)
            p = s.substr(0, i);
    }
    return p;
}

string TSource::getExportedDevSearchPattern() const {
    std::regex re("(.*)\\(\\*\\)");  // hokuto:20000/test/*/1(*)
    const std::string &s = rem_tghostproto(d->m_s); // test/*/1(*)
    std::smatch sm;
    if(std::regex_search(s, sm, re) && sm.size() > 1)
        return sm[1];
    return std::string();
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
std::string TSource::getArgOptions(size_t *pos_start, size_t *pos_end) const {
    arg_options ao;
    // capture special directives to interpret args
    // example a/b/c/d([sep(;)]arg1;arg2) sep: args separator
    const std::string &s = d->m_s;
    std::smatch sm;
    bool found = /*s.find('(') != std::string::npos &&*/ std::regex_search(s, sm, tsrc_regexps.get_args_re());
    if(found) {
        *pos_start = sm.position(1);
        *pos_end = *pos_start + sm.length(1);
    }
    //printf("getArgOptions: sm size %ld pos start %ld end %ld src '%s' siz %ld\n",
      //     sm.size(), *pos_start, *pos_end, s.c_str(), s.length());
    return found && sm.size() == 2 ? sm[1] : std::string();
}

/*!
 * \brief if the type is one of the database search methods, the search pattern
 *
 * \return
 */
string TSource::getSearchPattern() const {
    std::string s = rem_tghostproto(d->m_s);
    if(d->m_ty >= SrcDbDoma && d->m_ty <= SrcDbDevProps)
        return s;
    return std::string();
}

/*!
 * \brief returns true if the source represents an operation on the Tango Database
 */
bool TSource::isDbOp() const {
    return d->m_ty > SrcAttr && d->m_ty < SrcEndTypes;
}

TSource::Type TSource::getType() const {
    return d->m_ty;
}

TSource &TSource::operator=(const TSource &other) {
    if(this != &other) {
        d->m_s = other.d->m_s;
        d->m_ty = other.d->m_ty;
    }
    return *this;
}

bool TSource::operator ==(const TSource &other) const
{
    return d->m_s == other.d->m_s && d->m_ty == other.d->m_ty;
}

std::string TSource::toString() const
{
    char repr[512];
    snprintf(repr, 512, "TSource [%p] [name:\"%s\"] [device:\"%s\"] [point:\"%s\"] [type:%s] [args:\"%s\"]",
             this, d->m_s.c_str(), getDeviceName().c_str(), getPoint().c_str(), getTypeName(d->m_ty), getArgsString().c_str());
    return std::string(repr);
}

string TSource::remove_tgproto(const string &src) const {
    return src.size() > 0 && src.find("tango://") == 0 ? src.substr(strlen("tango://")) : src;
}

string TSource::remove_tghost(const string &src) const {
    std::string s(src);
    int pos = src.find(':'); // use regex only if host:PORT pattern is found
    if(pos != std::string::npos && pos > 0 && src.length() > pos + 1 && std::isdigit(src[pos+1]))
        s = std::regex_replace(src, tsrc_regexps.get_host_re(), "");
    return s;
}

string TSource::rem_tghostproto(const string &src) const
{
    std::string s = remove_tgproto(src);
    s = remove_tghost(s);
    return s;
}

string TSource::rem_args(const string &src) const {
    // capture everything within (\(.*\)), not minimal. check for '(' before using regex
    return src.find('(') != std::string::npos ? std::regex_replace(src, tsrc_regexps.get_args_re(), "") : src;
}

const char *TSource::getTypeName(Type t) const {
    if(t < SrcEndTypes)
        return tynames[t];
    return tynames[0]; // "SrcInvalid"
}

std::string TSource::m_get_args_delim(const string &arg_options) const {
    // find a custom separator, if specified at the beginning of the args section
    //  sep\((.*)\)
    // example: a/b/c-D([sep(:)]arg1:arg2:arg3)
    std::smatch sm;
    return arg_options.find("sep") != std::string::npos && std::regex_search(arg_options, sm, tsrc_regexps.get_separ_re()) && sm.size() == 2 ? sm[1] : std::string(",");
}

