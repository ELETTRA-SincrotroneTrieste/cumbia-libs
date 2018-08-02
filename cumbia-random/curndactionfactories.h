#ifndef CURNDACTIONFACTORIES_H
#define CURNDACTIONFACTORIES_H

#include <curndactioni.h>
#include <curndactionfactoryi.h>

class CuRNDActionReaderFactory : public CuRNDActionFactoryI
{
public:
    CuRNDActionReaderFactory();

    void setOptions(const CuData &o);

    virtual ~CuRNDActionReaderFactory();

    // CuTangoActionFactoryI interface
public:
    CuRNDActionI *create(const std::string &s, CumbiaWebSocket *c_rnd) const;

    CuRNDActionI::Type getType() const;

    bool isShareable() const;

private:
    CuData options;

};

#endif // CURNDACTIONFACTORIES_H
