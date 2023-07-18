#ifndef TSOURCE_H
#define TSOURCE_H

#include <string>
#include <list>
#include <cuvariant.h>
#include <regex>
#include <stdio.h>

using namespace std;

class TSourcePrivate;

// tghost:PORT regex ([A-Z-a-z0-9\-_\.\+~]+:\d+)
#define TGHOST_RE "([A-Z-a-z0-9\\-_\\.\\+~]+:\\d+)"

// tango device name regex
// used only if the source is not a command and the number of '/' is not 3
// otherwise string substr + find is enough
// ((?:tango://){0,1}(?:[A-Z-a-z0-9\-_\.\+~]+:\d*/){0,1}[A-Z-a-z0-9\-_\.\+~]+/[A-Z-a-z0-9\-_\.\+~]+/[A-Z-a-z0-9\-_\.\+~]+)
#define TG_DEV_RE "((?:tango://){0,1}(?:[A-Z-a-z0-9\\-_\\.\\+~]+:\\d*/){0,1}[A-Z-a-z0-9\\-_\\.\\+~]+/[A-Z-a-z0-9\\-_\\.\\+~]+/[A-Z-a-z0-9\\-_\\.\\+~]+)"

class regexps {
public:
    regexps() : dre(false), hre(false), fre(false),
        are(false), sre(false) {};

    std::regex& get_dev_re();
    std::regex& get_host_re();
    std::regex& get_freeprop_re();
    std::regex& get_args_re();
    std::regex& get_separ_re();

    // flags true if corresponding regex has been initialized
    bool dre, hre, fre, are, sre;

private:
    std::regex dev_re;
    std::regex host_re;
    std::regex freeprop_re;  // #(.*)#
    std::regex argopts_re;  // \(\[\s*(.*)\s*\]\s*.*\)
    std::regex args_re;
    std::regex separ_re;
};

struct arg_options {
    std::string separator;
};

class TSource
{
public:
    enum Type { SrcInvalid = 0, SrcCmd, SrcAttr,
                // search types
                SrcDbDoma, SrcDbFam, SrcDbMem, SrcDbAtts, SrcDbClassProps, SrcDbCmds, SrcDbAttInfo, SrcDbDevProps,
                // attribute, class, dev, free prop and command info
                SrcDbAProp, SrcDbGetCmdI, SrcDbClassProp, SrcDbDevProp, SrcDbFreeProp, SrcExportedDevs, SrcEndTypes };

    const char *tynames[SrcEndTypes + 1] = { "SrcInvalid", "SrcCmd", "SrcAttr", "SrcDbDoma", "SrcDbFam", "SrcDbMem",
                                             "SrcDbAtts","SrcDbClassProps",  "SrcDbCmds", "SrcDbAProps",  "SrcDbDevProps",
                                             "SrcDbAProp",  "SrcDbGetCmdI",  "SrcDbClassProp", "SrcDbDevProp", "SrcDbFreeProp", "SrcExportedDevs" "SrcEndTypes" };

    TSource();
    TSource(const std::string s);
    TSource(const TSource& other);
    ~TSource();

    std::string getDeviceName() const;
    std::string getDeviceNameOnly() const;
    std::string getPoint() const;
    std::string getName() const;
    std::string getTangoHost() const;
    std::string getFreePropNam() const;
    std::string getFreePropObj() const;
    std::string getSearchPattern() const;
    std::string getPropClassNam() const;
    std::string getExportedDevSearchPattern() const;
    std::string getArgOptions(size_t *pos_start, size_t *pos_end) const;

    bool isDbOp() const;

    void setArgs(const std::vector<std::string>& args);
    std::vector<string> getArgs() const;
    std::string getArgsString() const;
    std::vector<std::string> getPropNames() const;

    string toString() const;
    std::string remove_tgproto(const std::string& src) const;
    std::string remove_tghost(const std::string& src) const;
    std::string rem_tghostproto(const std::string& src) const;
    std::string rem_args(const std::string& src) const;

    const char* getTypeName(Type t) const;

    Type getType() const;

    TSource & operator=(const TSource& other);
    bool operator ==(const TSource &other) const;

private:
    TSourcePrivate *d;
    TSource::Type m_get_ty(const std::string &src) const;
    std::string m_get_args_delim(const std::string& args) const;
};

#endif // TSOURCE_H
