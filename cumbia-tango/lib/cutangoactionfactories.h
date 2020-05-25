#ifndef CUTANGOACTIONFACTORIES_H
#define CUTANGOACTIONFACTORIES_H

#include <cutangoactionfactoryi.h>
#include <cutangoactioni.h>
#include <cuvariant.h>
#include <cutreader.h>

class CumbiaTango;

class CuTangoReaderFactory : public CuTangoActionFactoryI
{
public:
    CuTangoReaderFactory();
    void setOptions(const CuData &o);
    virtual ~CuTangoReaderFactory();

    // CuTangoActionFactoryI interface
public:
    CuTangoActionI *create(const std::string &s, CumbiaTango *ct) const;
    CuTangoActionI::Type getType() const;
    bool isShareable() const;

private:
    CuData options;
};

class CuTangoWriterFactory : public CuTangoActionFactoryI
{
public:
    CuTangoWriterFactory();
    virtual ~CuTangoWriterFactory();

    void setOptions(const CuData &o);
    void setWriteValue(const CuVariant &write_val);

    /*! \brief stores the result of get_attribute_config or get_command_info
     *
     * This methods allows to save configuration data obtained from the Tango database once
     * at setup time.
     *
     * @param CuData data decoded from either get_attribute_config or get_command_info calls
     */
    void setConfiguration(const CuData& configuration);

    // CuTangoActionFactoryI interface
public:
    CuTangoActionI *create(const std::string &s, CumbiaTango *ct) const;
    CuTangoActionI::Type getType() const;
    bool isShareable() const;
private:
    CuVariant m_write_val;
    CuData options, m_configuration;
};

class CuTConfFactoryBase : public CuTangoActionFactoryI
{
public:
    virtual ~CuTConfFactoryBase();
    void setOptions(const CuData &o);
    CuData options() const;
private:
    std::vector<std::string> m_props;
    CuData m_options;
};

class CuTReaderConfFactory : public CuTConfFactoryBase
{
public:
    CuTangoActionI *create(const std::string &s, CumbiaTango *ct) const;
    CuTangoActionI::Type getType() const;
};

class CuTWriterConfFactory : public CuTConfFactoryBase
{
public:
    CuTangoActionI *create(const std::string &s, CumbiaTango *ct) const;
    CuTangoActionI::Type getType() const;
};

class CuTaDbFactoryPrivate;

class CuTaDbFactory : public CuTangoActionFactoryI
{
public:
    CuTaDbFactory();
    ~CuTaDbFactory();

    CuTangoActionI *create(const std::string &s, CumbiaTango *ct) const;
    CuTangoActionI::Type getType() const;
    void setOptions(const CuData &o);
    CuData options() const;

private:
    CuTaDbFactoryPrivate* d;
};

#endif // CUTANGOACTIONFACTORIES_H
