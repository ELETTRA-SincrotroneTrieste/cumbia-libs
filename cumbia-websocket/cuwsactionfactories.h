#ifndef CUWSACTIONFACTORIES_H
#define CUWSACTIONFACTORIES_H

#include <cuwsactioni.h>
#include <cuwsactionfactoryi.h>

class CuWSClient;

class CuWSActionReaderFactory : public CuWSActionFactoryI
{
public:
    void setOptions(const CuData &o);

    virtual ~CuWSActionReaderFactory();

    // CuTangoActionFactoryI interface
public:
    CuWSActionI *create(const std::string &s, CuWSClient *cli, const QString& http_addr) const;

    CuWSActionI::Type getType() const;

private:
    CuData options;

};

class CuWSActionWriterFactory : public CuWSActionFactoryI
{
public:
    void setConfiguration(const CuData &conf);
    void setWriteValue(const CuVariant &write_val);

    virtual ~CuWSActionWriterFactory();


    // CuTangoActionFactoryI interface
public:
    CuWSActionI *create(const std::string &s, CuWSClient *cli, const QString& http_addr) const;

    CuWSActionI::Type getType() const;

private:
    CuData configuration;
    CuVariant m_write_val;
};

class CuWSActionConfFactory : public CuWSActionFactoryI
{
public:
    void setOptions(const CuData &o);

    virtual ~CuWSActionConfFactory();

    // CuTangoActionFactoryI interface
public:
    CuWSActionI *create(const std::string &s, CuWSClient *cli, const QString& http_addr) const;

    CuWSActionI::Type getType() const;

private:
    CuData options;

};

class CuWsActionWriterConfFactory : public CuWSActionConfFactory {
    CuWSActionI::Type getType() const;
};

#endif // CUWSACTIONFACTORIES_H
