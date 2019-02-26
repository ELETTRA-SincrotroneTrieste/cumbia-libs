#ifndef CUFORMULASREADER_H
#define CUFORMULASREADER_H

#include <QString>
#include <QObject>
#include <cucontrolsreader_abs.h>
#include <cucontrolsfactories_i.h>
#include <cudata.h>
#include <cudataquality.h>

class CuFormulaReaderFactoryPrivate;
class CumbiaPool;
class CuControlsFactoryPool;

class CuFormulaReaderFactory : public CuControlsReaderFactoryI
{
    // CuControlsWriterFactoryI interface
public:
    CuFormulaReaderFactory(CumbiaPool *cu_poo, const CuControlsFactoryPool &fpool);

    virtual ~CuFormulaReaderFactory();

    CuControlsReaderA *create(Cumbia *c, CuDataListener *l) const;

    void setOptions(const CuData& o);

    CuData getOptions() const;

    // CuControlsReaderFactoryI interface
public:
    CuControlsReaderFactoryI *clone() const;

private:
    CuFormulaReaderFactoryPrivate *d;
};

class CuFormulaReaderPrivate;

class CuFormulaReader : public QObject, public CuControlsReaderA
{
    Q_OBJECT
public:
    CuFormulaReader(Cumbia *c, CuDataListener *l, CumbiaPool *cu_poo, const CuControlsFactoryPool &fpool);

    ~CuFormulaReader();

    bool error() const;

    QString message() const;

    CuDataQuality combinedQuality() const;

    std::string combinedMessage() const;

    std::vector<bool> errors() const;

    // CuControlsReaderA interface
public:
    void setSource(const QString &s);
    QString source() const;
    void unsetSource();
    void sendData(const CuData &d);
    void getData(CuData &d_ino) const;

private slots:
    void onNewData(const CuData & da);

private:
    CuFormulaReaderPrivate *d;

    bool m_allValuesValid() const;

    void m_disposeWatchers();
};

#endif // CUFORMULASREADER_H
