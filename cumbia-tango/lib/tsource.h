#ifndef TSOURCE_H
#define TSOURCE_H

#include <string>
#include <list>
#include <cuvariant.h>

using namespace std;

class TSourcePrivate;

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

    bool isDbOp() const;

    void setArgs(const std::vector<std::string>& args);
    std::vector<string> getArgs() const;
    std::string getArgsString() const;
    std::vector<std::string> getPropNames() const;

    string toString() const;
    std::string remove_tgproto(const std::string& src) const;
    std::string remove_tghost(const std::string& src) const;
    std::string rem_tghostproto(const std::string& src) const;

    const char* getTypeName(Type t) const;

    Type getType() const;

    TSource & operator=(const TSource& other);
    bool operator ==(const TSource &other) const;

private:
    TSourcePrivate *d;
    TSource::Type m_get_ty(const std::string &src) const;
};

#endif // TSOURCE_H
