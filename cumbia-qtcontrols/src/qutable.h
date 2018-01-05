#ifndef QUTABLE_H
#define QUTABLE_H

#include <eflag.h>
#include <cudatalistener.h>
#include <cucontexti.h>

class QuTablePrivate;
class Cumbia;
class CuControlsReaderFactoryI;
class CumbiaPool;
class CuControlsFactoryPool;
class CuContext;

/** \brief A widget to organise data in a table
 *
 * A default set of options for the object is configured when QuTable is initialised.
 * It includes numRows, numColumns, displayMask, trueColours, falseColours,
 * trueStrings, falseStrings. If they are provided by the underline engine, they are
 * used to configure the table.
 */
class QuTable : public EFlag, public CuDataListener, public CuContextI
{
    Q_OBJECT
    Q_PROPERTY(QString source READ source WRITE setSource DESIGNABLE true)

public:
    QuTable(QWidget *w, Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac);

    QuTable(QWidget *w, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool);

    virtual ~QuTable();

    QString source() const;

    int maximumLength() const;

    void getData(CuData &d_inout) const;

    CuContext *getContext() const;

    void setOptions(const CuData& data);

public slots:
    void setSource(const QString& s);

    void unsetSource();

signals:
    void newData(const CuData&da);

protected:

    void configure(const CuData &da);
private:

    void m_init();

    QuTablePrivate *d;

    // CuDataListener interface
public:
    void onUpdate(const CuData &d);
};

#endif // QUTABLE_H
