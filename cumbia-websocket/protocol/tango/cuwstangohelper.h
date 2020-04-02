#ifndef TANGOHELPER_H
#define TANGOHELPER_H

#include <cuwsprotocolhelper_i.h>

class CuWsTangoHelper : public ProtocolHelper_I
{
public:
    CuWsTangoHelper();

    ~CuWsTangoHelper();

    // ProtocolHelper_I interface
public:
    virtual int dataFormatStrToInt(const std::string &fmt) const;
    virtual std::string dataFormatToStr(int fmt) const;
    virtual std::string dataTypeToStr(int dt) const;
    virtual int dataTypeStrToInt(const std::string &dt) const;

    /*!
     * \brief srcPatterns allows to get Tango specific src patterns without
     *        the Tango dependency
     * \return src patterns (see CuTangoWorld::srcPatterns)
     */
    std::vector<std::string> srcPatterns() const;
};

#endif // TANGOHELPER_H
