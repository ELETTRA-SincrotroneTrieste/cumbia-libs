#ifndef CULINKCONTROL_H
#define CULINKCONTROL_H

class Cumbia;
class CumbiaPool;
class CuControlsReaderFactoryI;
class CuControlsWriterFactoryI;
class CuControlsReaderA;
class CuDataListener;

#include <string>
#include <cucontrolsfactorypool.h>

/** \brief CuLinkControl stores a small set of objects to help create readers and writers.
 *
 * This can be considered a helper class for private management of Cumbia and reader/writer
 * factories within reader or writer classes.
 *
 * \par Usage
 * CuLinkControl can be created
 * \li passing specific Cumbia and reader (writer) factory instances;
 * \li passing CumbiaPool and CuControlsFactoryPool objects.
 *
 * When make_reader is called, either the specific Cumbia and reader factory are used to create a
 * reader or implementations of Cumbia and reader factory are picked from the pools provided.
 */
class CuLinkControl
{
public:
    CuLinkControl(Cumbia *cumbia, const CuControlsReaderFactoryI &r_factory);

    CuLinkControl(Cumbia *cumbia, const CuControlsWriterFactoryI &w_factory);

    CuLinkControl(CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool);

    virtual ~CuLinkControl();

    Cumbia* cu;

    CumbiaPool *cu_pool;

    CuControlsReaderFactoryI *r_factory;

    CuControlsWriterFactoryI *w_factory;

    CuControlsFactoryPool ctrl_factory_pool;

    bool isAuto() const;

    CuControlsReaderA *make_reader(const std::string &s, CuDataListener* datal) const;

};

#endif // CULINKCONTROL_H
