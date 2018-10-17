#ifndef QULED_H
#define QULED_H

#include <quledbase.h>
#include <cudatalistener.h>
#include <cucontexti.h>

class QuLedPrivate;
class Cumbia;
class CuControlsReaderFactoryI;
class CumbiaPool;
class CuControlsFactoryPool;
class CuContext;

/*! \brief a *led* that takes a color according to a value read from a source.
 *
 * \ingroup outputw
 *
 * The class derives from QuLedBase. Several configuration options for the shape
 * and color mode of the led can be set through QuLedBase interface.
 *
 * CuContext is used to communicate through a given engine with the source of data.
 */
class QuLed : public QuLedBase, public CuDataListener, public CuContextI
{
    Q_OBJECT
    Q_PROPERTY(QString source READ source WRITE setSource DESIGNABLE true)

public:
    QuLed(QWidget *w, Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac);

    QuLed(QWidget *w, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool);

    virtual ~QuLed();

    QString source() const;

    CuContext *getContext() const;

public slots:
    void setSource(const QString& s);

    void unsetSource();

signals:
    void newData(const CuData&);
    void linkStatsRequest(QWidget *myself, CuContextI *myself_as_cwi);

protected:

    void contextMenuEvent(QContextMenuEvent *e);

private:
    QuLedPrivate *d;

    void m_init();

    // CuTangoListener interface
public:
    void onUpdate(const CuData &d);
};

#endif // QULED_H
