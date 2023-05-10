#ifndef CUHTTPTANGOSRC_H
#define CUHTTPTANGOSRC_H

#include <string>
#include <list>
#include <cuvariant.h>

using namespace std;

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
    std::string remove_tgproto(const std::string& src) const;
    std::string remove_tghost(const std::string& src) const;
    std::string rem_tghostproto(const std::string& src) const;
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
