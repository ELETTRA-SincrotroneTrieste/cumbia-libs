#ifndef CUTANGOACTIONFACTORYI_H
#define CUTANGOACTIONFACTORYI_H

class CumbiaTango;

#include <string>
#include <cutangoactioni.h>

class CuTangoActionFactoryI
{
public:
    CuTangoActionFactoryI() {}

    virtual CuTangoActionI *create(const std::string& source, CumbiaTango *ct) const = 0;

    /** \brief Return the type of action that the factory creates
     *
     * @return the type of action that the factory creates
     */
    virtual CuTangoActionI::Type getType() const = 0;
};

#endif // CUTANGOACTIONFACTORYI_H
