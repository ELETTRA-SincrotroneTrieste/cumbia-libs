#ifndef CUWSACTIONFACTORIES_H
#define CUWSACTIONFACTORIES_H

#include <cuwsactioni.h>
#include <cuwsactionfactoryi.h>

class CuWSActionReaderFactory : public CuWSActionFactoryI
{
public:
    CuWSActionReaderFactory();

    void setOptions(const CuData &o);

    virtual ~CuWSActionReaderFactory();

    // CuTangoActionFactoryI interface
public:
    CuWSActionI *create(const std::string &s, CumbiaWebSocket *c_ws) const;

    CuWSActionI::Type getType() const;

    bool isShareable() const;

private:
    CuData options;

};

#endif // CUWSACTIONFACTORIES_H
