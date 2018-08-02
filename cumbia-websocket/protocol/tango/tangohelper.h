#ifndef TANGOHELPER_H
#define TANGOHELPER_H

#include <protocolhelper_i.h>

class TangoHelper : public ProtocolHelper_I
{
public:
    TangoHelper();

    ~TangoHelper();

    // ProtocolHelper_I interface
public:
    virtual int dataFormatStrToInt(const std::string &fmt) const;
    virtual std::string dataFormatToStr(int fmt) const;
    virtual std::string dataTypeToStr(int dt) const;
    virtual int dataTypeStrToInt(const std::string &dt) const;
};

#endif // TANGOHELPER_H
