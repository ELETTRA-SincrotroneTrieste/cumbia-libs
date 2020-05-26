#ifndef TSOURCE_H
#define TSOURCE_H

#include <string>
#include <list>
#include <cuvariant.h>

using namespace std;

class TSource
{
public:
    enum Type { SrcInvalid = 0, SrcCmd, SrcAttr,
                // search types
                SrcDbDoma, SrcDbFam, SrcDbMem, SrcDbAtts, SrcDbClassProps, SrcDbCmds, SrcDbAProps, SrcDbDevProps,
                // attribute, class, dev, free prop and command info
                SrcDbAProp, SrcDbGetCmdI, SrcDbClassProp, SrcDbDevProp, SrcDbFreeProp, SrcEndTypes };

    const char *tynames[SrcEndTypes + 1] = { "SrcInvalid", "SrcCmd", "SrcAttr", "SrcDbDoma", "SrcDbFam", "SrcDbMem",
                                             "SrcDbAtts","SrcDbClassProps",  "SrcDbCmds", "SrcDbAProps",  "SrcDbDevProps",
                                             "SrcDbAProp",  "SrcDbGetCmdI",  "SrcDbClassProp", "SrcDbDevProp", "SrcDbFreeProp", "SrcEndTypes" };

    TSource();
    TSource(const std::string s);
    TSource(const TSource& other);

    std::string getDeviceName() const;
    std::string getPoint() const;
    std::string getName() const;
    std::string getTangoHost(const string &src) const;
    std::string getPropNam() const;
    std::string getSearchPattern() const;
    std::string getPropClassNam() const;

    bool isDbOp() const;

    std::vector<string> getArgs() const;

    string toString() const;
    std::string remove_tgproto(const std::string& src) const;
    std::string remove_tghost(const std::string& src) const;
    std::string rem_tghostproto(const std::string& src) const;

    const char* getTypeName(Type t) const;

    Type getType() const;

    TSource & operator=(const TSource& other);

    bool operator ==(const TSource &other) const;

    std::string getArgsString() const;

private:
    string m_s;
    Type m_ty;

    TSource::Type m_get_ty(const std::string &src) const;
};

#endif // TSOURCE_H
