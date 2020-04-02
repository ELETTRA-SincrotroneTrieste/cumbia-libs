#ifndef PROTOCOLHELPER_I_H
#define PROTOCOLHELPER_I_H

#include <string>
#include <vector>

class ProtocolHelper_I
{
public:

    virtual ~ProtocolHelper_I() {}

    virtual int dataFormatStrToInt(const std::string& fmt) const = 0;

    virtual std::string dataFormatToStr(int fmt)  const = 0;

    virtual std::string dataTypeToStr(int dt) const = 0;

    virtual int dataTypeStrToInt(const std::string &dt ) const = 0;

    virtual std::vector<std::string> srcPatterns() const = 0;
};

#endif // PROTOCOLHELPER_I_H
