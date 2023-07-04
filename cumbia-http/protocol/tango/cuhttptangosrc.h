#ifndef CUHTTPTANGOSRC_H
#define CUHTTPTANGOSRC_H

#include <string>
#include <list>
#include <cuvariant.h>
#include <regex>

using namespace std;

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

class CuHttpTangoSrc
{
public:
    enum Type { SrcInvalid = 0, SrcCmd, SrcAttr,
                // search types
                SrcDbDoma, SrcDbFam, SrcDbMem, SrcDbAtts, SrcDbClassProps, SrcDbCmds, SrcDbAttInfo, SrcDbDevProps,
                // attribute, class, dev, free prop and command info
                SrcDbAProp, SrcDbGetCmdI, SrcDbClassProp, SrcDbDevProp, SrcDbFreeProp, SrcEndTypes };

    const char *tynames[SrcEndTypes + 1] = { "SrcInvalid", "SrcCmd", "SrcAttr", "SrcDbDoma", "SrcDbFam", "SrcDbMem",
                                             "SrcDbAtts","SrcDbClassProps",  "SrcDbCmds", "SrcDbAProps",  "SrcDbDevProps",
                                             "SrcDbAProp",  "SrcDbGetCmdI",  "SrcDbClassProp", "SrcDbDevProp", "SrcDbFreeProp", "SrcEndTypes" };

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
