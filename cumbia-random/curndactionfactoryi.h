#ifndef CURNDACTIONFACTORYI_H
#define CURNDACTIONFACTORYI_H

class CumbiaRandom;

#include <string>
#include <curndactioni.h>

/** \brief Factory to instantiate CuTangoActionI implementations, such as readers, writers and
 *         attribute configuration actions.
 *
 * Notable implementations
 *
 * \li CuTangoReaderFactory
 * \li CuTangoWriterFactory
 * \li CuTangoAttConfFactory
 *
 */
class CuRNDActionFactoryI
{
public:
    CuRNDActionFactoryI() {}

    /**
     * Pure virtual method that must be implemented by subclasses to return a valid CuTangoActionI
     * instance.
     *
     * @param source a string identifying a source to connect to
     * @param ct a pointer to a CumbiaRandom instance
     */
    virtual CuRNDActionI *create(const std::string& source, CumbiaRandom *ct) const = 0;

    /** \brief Return the type of action that the factory creates
     *
     * @return the type of action that the factory creates
     */
    virtual CuRNDActionI::Type getType() const = 0;
};

#endif // CUTANGOACTIONFACTORYI_H
