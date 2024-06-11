#ifndef CUCONTEXT_H
#define CUCONTEXT_H

class Cumbia;
class CumbiaPool;
class CuEngineAccessor;
class CuControlsReaderFactoryI;
class CuControlsWriterFactoryI;
class CuControlsReaderA;
class CuControlsPropertyReaderA;
class CuControlsWriterA;
class CuDataListener;
class CuLinkStats;

#include <string>
#include <cucontrolsfactorypool.h>
#include <cudata.h>
#include <QList>

class CuContextPrivate;

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
 * If CuData is passed to make_reader (or make_writer), it is used to configure
 * the options of the underlying engine. CuData key/value set is specific to the engine used.
 *
 * \par Options
 * The setOptions method can be used to configure *how* the readers and writers created by CuContext
 * are set up. setOptions accepts a CuData bundle with settings organised in key/value pairs.
 *
 * \par Relevant key/value pairs
 * \li *no-properties* [bool, default: false]. If true, skip attribute configuration.
 * \li *fetch_props* [std::vector<std::string>, default: empty] list of attribute properties to
 *     fetch from the database during configuration. The *no-properties* key must be left to false
 *     (default).
 */
class CuContext
{
public:
    CuContext(Cumbia *cumbia, const CuControlsReaderFactoryI &r_factory);
    CuContext(Cumbia *cumbia, const CuControlsWriterFactoryI &w_factory);
    CuContext(CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool);
    CuContext(CuEngineAccessor *a);

    virtual ~CuContext();

    bool isAuto() const;

    CuControlsReaderA *replace_reader(const std::string &s, CuDataListener* datal);
    CuControlsWriterA *replace_writer(const std::string &s, CuDataListener *datal);
    CuControlsReaderA *add_reader(const std::string &s, CuDataListener* datal);
    CuControlsWriterA *add_writer(const std::string &s, CuDataListener *datal);

    CuControlsPropertyReaderA* get_property_reader(const std::string& s, CuDataListener *datal);

    void disposeReader(const std::string &src = std::string());
    void disposeWriter(const std::string &src =  std::string());

    void setOptions(const CuData& options);
    void sendData(const CuData& data);
    void sendData(const QList<CuData> &data);
    void getData(CuData& d_inout);
    void getData(QList<CuData> &in_datalist);

    CuData options() const;

    CuControlsReaderA *getReader() const;
    CuControlsWriterA *getWriter() const;
    CuControlsReaderA *findReader(const std::string& srcnam);
    CuControlsWriterA *findWriter(const std::string& targetnam);
    CuControlsReaderFactoryI *getReaderFactoryI() const;
    CuControlsWriterFactoryI *getWriterFactoryI() const;
    CuControlsFactoryPool getControlsFactoryPool() const;
    CuLinkStats *getLinkStats() const;

    QList<CuControlsReaderA*> readers() const;
    QList<CuControlsWriterA *> writers() const;

    Cumbia* cumbia() const;
    CumbiaPool *cumbiaPool() const;

private:

    CuContextPrivate *d;

    CuControlsReaderA *m_make_reader(const std::string &s, CuDataListener* datal) const;
    CuControlsWriterA *m_make_writer(const std::string &s, CuDataListener* datal) const;

};

#endif // CULINKCONTROL_H
