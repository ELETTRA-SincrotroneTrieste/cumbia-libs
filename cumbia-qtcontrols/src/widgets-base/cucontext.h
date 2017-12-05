#ifndef CUCONTEXT_H
#define CUCONTEXT_H

class Cumbia;
class CumbiaPool;
class CuControlsReaderFactoryI;
class CuControlsWriterFactoryI;
class CuControlsReaderA;
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
 */
class CuContext
{
public:
    CuContext(Cumbia *cumbia, const CuControlsReaderFactoryI &r_factory);

    CuContext(Cumbia *cumbia, const CuControlsWriterFactoryI &w_factory);

    CuContext(CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool);

    virtual ~CuContext();

    bool isAuto() const;

    CuControlsReaderA *replace_reader(const std::string &s, CuDataListener* datal);

    CuControlsWriterA *replace_writer(const std::string &s, CuDataListener *datal);

    CuControlsReaderA *add_reader(const std::string &s, CuDataListener* datal);

    CuControlsWriterA *add_writer(const std::string &s, CuDataListener *datal);

    void unlinkReader(const std::string &src = std::string());

    void unlinkWriter(const std::string &src =  std::string());

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
