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

    // CuTangoActionFactoryI interface
public:
    CuTangoActionI *create(const std::string &s, CumbiaTango *ct) const;

    CuTangoActionI::Type getType() const;

    bool isShareable() const;

private:
    CuVariant m_write_val;
    CuData options;
};

class CuTangoAttConfFactory : public CuTangoActionFactoryI
{
public:
    CuTangoAttConfFactory();

    virtual ~CuTangoAttConfFactory();

    void setOptions(const CuData &o);

    // CuTangoActionFactoryI interface
public:
    CuTangoActionI *create(const std::string &s, CumbiaTango *ct) const;

    CuTangoActionI::Type getType() const;

private:
    std::vector<std::string> m_props;
    CuData options;
};

#endif // CUTANGOACTIONFACTORIES_H
