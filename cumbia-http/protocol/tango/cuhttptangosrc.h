#ifndef CUHTTPTANGOSRC_H
#define CUHTTPTANGOSRC_H

#include <string>
#include <list>
#include <cuvariant.h>
#include <regex>

using namespace std;

// tghost:PORT regex ([A-Z-a-z0-9\-_\.\+~]+:\d+)
#define TGHOST_RE "([A-Z-a-z0-9\\-_\\.\\+~]+:\\d+)"

// tango device name regex
// used only if the source is not a command and the number of '/' is not 3
// otherwise string substr + find is enough
// ((?:tango://){0,1}(?:[A-Z-a-z0-9\-_\.\+~]+:\d*/){0,1}[A-Z-a-z0-9\-_\.\+~]+/[A-Z-a-z0-9\-_\.\+~]+/[A-Z-a-z0-9\-_\.\+~]+)
#define TG_DEV_RE "((?:tango://){0,1}(?:[A-Z-a-z0-9\\-_\\.\\+~]+:\\d*/){0,1}[A-Z-a-z0-9\\-_\\.\\+~]+/[A-Z-a-z0-9\\-_\\.\\+~]+/[A-Z-a-z0-9\\-_\\.\\+~]+)"

class tg_http_regexps {
public:
    static std::regex& get_dev_re() {
        static std::regex dev_re(TG_DEV_RE);
        return dev_re;
    }
    static std::regex& get_host_re() {
        static std::regex host_re(TGHOST_RE);
        return host_re;
    }
    static std::regex& get_freeprop_re() {
        static std::regex freeprop_re = std::regex("#(.*)#");;
        return freeprop_re;
    }
    static std::regex& get_args_re() {
        static std::regex args_re =std::regex("\\((?:\\[\\s*(.*)\\s*\\]\\s*){0,1}.*\\)");
        return args_re;
    }
    static std::regex& get_separ_re() {
        static std::regex separ_re = std::regex("sep\\((.*)\\)");
        return separ_re;
    }
};

class CuHttpTangoSrc
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

    CuHttpTangoSrc();
    CuHttpTangoSrc(const std::string s);
    CuHttpTangoSrc(const CuHttpTangoSrc& other);

    std::string getDeviceName() const;
    std::string getDeviceNameOnly() const;
    std::string getPoint() const;
    std::string getName() const;
    std::string getTangoHost() const;
    std::string getFreePropNam() const;
    std::string getFreePropObj() const;
    std::string getSearchPattern() const;
    std::string getPropClassNam() const;
    std::string getArgOptions(size_t *pos_start, size_t *pos_end) const;

    bool isDbOp() const;

    std::vector<string> getArgs() const;
    std::string getArgsString() const;
    std::vector<std::string> getPropNames() const;

    string toString() const;
    std::string rem_tghost(const std::string& src) const;
    std::string rem_httpproto(const std::string& src) const;
    std::string rem_args(const std::string &src) const;

    const char* getTypeName(Type t) const;
    Type getType() const;

    CuHttpTangoSrc & operator=(const CuHttpTangoSrc& other);

    bool operator ==(const CuHttpTangoSrc &other) const;
    bool isValid() const;

private:
    string m_s;
    Type m_ty;

    CuHttpTangoSrc::Type m_get_ty(const std::string &src) const;
    std::string m_get_args_delim(const std::string& args) const;
};


#endif // CUHTTPTANGOSRC_H
