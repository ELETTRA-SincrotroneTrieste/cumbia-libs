#ifndef CUEPICSACTIONFACTORIES_H
#define CUEPICSACTIONFACTORIES_H

#include <cuepactionfactoryi.h>
#include <cuepactioni.h>
#include <cuvariant.h>
#include <cumonitor.h>
#include <cuepreadoptions.h>

class CumbiaEpics;

class CuEpicsReaderFactory : public CuEpicsActionFactoryI
{
public:
    CuEpicsReaderFactory();

    void setOptions(const CuData& o);

    virtual ~CuEpicsReaderFactory();

    // CuEpicsActionFactoryI interface
public:
    CuEpicsActionI *create(const std::string &s, CumbiaEpics *ct) const;

    CuEpicsActionI::Type getType() const;

    bool isShareable() const;

private:
    CuData options;

};

class CuEpicsWriterFactory : public CuEpicsActionFactoryI
{
public:
    CuEpicsWriterFactory();

    virtual ~CuEpicsWriterFactory() {}

    void setWriteValue(const CuVariant &write_val);

    // CuEpicsActionFactoryI interface
public:
    CuEpicsActionI *create(const std::string &s, CumbiaEpics *ct) const;

    CuEpicsActionI::Type getType() const;

    bool isShareable() const;

private:
    CuVariant m_write_val;
};

class CuEpicsPropertyFactory : public CuEpicsActionFactoryI
{
public:
    CuEpicsPropertyFactory();

    virtual ~CuEpicsPropertyFactory() {}

    void setDesiredPVProperties(const std::vector<std::string> props);

    // CuEpicsActionFactoryI interface
public:
    CuEpicsActionI *create(const std::string &s, CumbiaEpics *ce) const;

    CuEpicsActionI::Type getType() const;

private:
    std::vector<std::string> m_props;
    bool m_fetchAttHistory;
};

#endif // CUEPICSACTIONFACTORIES_H
