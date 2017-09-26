#ifndef CUTANGOACTIONFACTORIES_H
#define CUTANGOACTIONFACTORIES_H

#include <cutangoactionfactoryi.h>
#include <cutangoactioni.h>
#include <cuvariant.h>
#include <cutreader.h>
#include <cutangoreadoptions.h>

class CumbiaTango;

class CuTangoReaderFactory : public CuTangoActionFactoryI
{
public:
    CuTangoReaderFactory();

    void setOptions(const CuTangoReadOptions& o);

    virtual ~CuTangoReaderFactory();

    // CuTangoActionFactoryI interface
public:
    CuTangoActionI *create(const std::string &s, CumbiaTango *ct) const;

    CuTangoActionI::Type getType() const;

    bool isShareable() const;

private:
    CuTangoReadOptions *options;

};

class CuTangoWriterFactory : public CuTangoActionFactoryI
{
public:
    CuTangoWriterFactory();

    void setWriteValue(const CuVariant &write_val);

    // CuTangoActionFactoryI interface
public:
    CuTangoActionI *create(const std::string &s, CumbiaTango *ct) const;

    CuTangoActionI::Type getType() const;

    bool isShareable() const;

private:
    CuVariant m_write_val;
};

class CuTangoAttConfFactory : public CuTangoActionFactoryI
{
public:
    CuTangoAttConfFactory();

    void fetchAttributeHistory(bool fetch);

    void setDesiredAttributeProperties(const std::vector<std::string> props);

    // CuTangoActionFactoryI interface
public:
    CuTangoActionI *create(const std::string &s, CumbiaTango *ct) const;

    CuTangoActionI::Type getType() const;

private:
    std::vector<std::string> m_props;
    bool m_fetchAttHistory;
};

#endif // CUTANGOACTIONFACTORIES_H
