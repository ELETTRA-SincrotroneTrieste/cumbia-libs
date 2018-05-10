#ifndef CUEPICSACTIONFACTORYI_H
#define CUEPICSACTIONFACTORYI_H

class CumbiaEpics;

#include <string>
#include <cuepactioni.h>

class CuEpicsActionFactoryI
{
public:
    CuEpicsActionFactoryI() {}

    virtual ~CuEpicsActionFactoryI() {}

    virtual CuEpicsActionI *create(const std::string& source, CumbiaEpics *ct) const = 0;

    /** \brief Return the type of action that the factory creates
     *
     * @return the type of action that the factory creates
     */
    virtual CuEpicsActionI::Type getType() const = 0;
};

#endif // CUEPICSACTIONFACTORYI_H
