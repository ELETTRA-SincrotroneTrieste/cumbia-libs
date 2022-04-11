#ifndef CUHTTPACTIONFACTORIES_H
#define CUHTTPACTIONFACTORIES_H

#include <cuhttpactiona.h>
#include <cuhttpactionfactoryi.h>

class QNetworkAccessManager;
class CuHttpChannelReceiver;
class CuHttpAuthManager;
class CuHTTPReaderFactoryPrivate;

class CuHTTPActionReaderFactory : public CuHTTPActionFactoryI {
public:
    CuHTTPActionReaderFactory(bool single_shot);
    virtual ~CuHTTPActionReaderFactory();

    void mergeOptions(const CuData &o);
    // CuTangoActionFactoryI interface
    std::string getMethod() const;
    CuHTTPActionFactoryI* clone() const;
    CuData options() const;
private:
    CuHTTPReaderFactoryPrivate *d;
};

class CuHTTPActionWriterFactory : public CuHTTPActionFactoryI
{
public:
    virtual ~CuHTTPActionWriterFactory();

    void setConfiguration(const CuData &conf);
    void setWriteValue(const CuVariant &write_val);
    std::string getMethod() const;
    CuHTTPActionFactoryI* clone() const;
    CuData options() const;
private:
    CuData o;
    CuVariant m_write_val;
};

class CuHTTPActionConfFactory : public CuHTTPActionFactoryI
{
public:
    void setOptions(const CuData &o);
    void mergeOptions(const CuData &o);
    virtual ~CuHTTPActionConfFactory();

    // CuTangoActionFactoryI interface
    std::string getMethod() const;
    CuHTTPActionFactoryI* clone() const;
    CuData options() const;

private:
    CuData m_o;
};

class CuHTTPActionEditFactory : public CuHTTPActionFactoryI
{
public:
    void setOptions(const CuData &o);
    virtual ~CuHTTPActionEditFactory();

    std::string getMethod() const;
    CuHTTPActionFactoryI* clone() const;
    CuData options() const;

private:
    CuData m_ed_o;
};


#endif // CUHTTPACTIONFACTORIES_H
