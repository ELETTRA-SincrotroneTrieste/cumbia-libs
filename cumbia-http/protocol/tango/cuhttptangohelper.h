#ifndef CUHTTPTANGOHELPER_H
#define CUHTTPTANGOHELPER_H

#include <cuhttpprotocolhelper_i.h>

class CuHttpTangoHelper : public ProtocolHelper_I
{
public:
    CuHttpTangoHelper();

    ~CuHttpTangoHelper();

    // ProtocolHelper_I interface
public:
    virtual int dataFormatStrToInt(const std::string &fmt) const;
    virtual std::string dataFormatToStr(int fmt) const;
    virtual std::string dataTypeToStr(int dt) const;
    virtual int dataTypeStrToInt(const std::string &dt) const;

    /*!
     * \brief srcPatterns allohttp to get Tango specific src patterns without
     *        the Tango dependency
     * \return src patterns (see CuTangoWorld::srcPatterns)
     */
    std::vector<std::string> srcPatterns() const;
};

#endif // CUHTTPTANGOHELPER_H
