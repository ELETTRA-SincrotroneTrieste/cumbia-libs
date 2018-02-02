#ifndef QULABEL_H
#define QULABEL_H

#include <esimplelabel.h>
#include <cudatalistener.h>
#include <cucontexti.h>
#include <cudata.h>

class QuLabelPrivate;
class Cumbia;
class CumbiaPool;
class CuControlsReaderFactoryI;
class CuControlsFactoryPool;
class CuContext;
class CuLinkStats;

/** \brief A label derived from Qt QLabel to display boolean values, strings, scalars and even vectors.
 *
 * \ingroup outputw
 *
 * \li Colors and strings can be associated to boolean values.
 * \li Scalar numbers and strings can be displayed by QuLabel.
 * \li Vectors are usually displayed only partially, according to the number of maximum characters allowed by
 *     setMaximumLength
 *
 * Connection is initiated with setSource. When new data arrives, it is displayed and the newData convenience
 * signal is emitted.
 *
 * getContext returns a pointer to the CuContext used as a delegate for the connection.
 *
 */
class QuLabel : public ESimpleLabel, public CuDataListener, public CuContextI
{
    Q_OBJECT
    Q_PROPERTY(QString source READ source WRITE setSource DESIGNABLE true)

public:
    QuLabel(QWidget *w, Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac);

    QuLabel(QWidget *w, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool);

    virtual ~QuLabel();

    QString source() const;

    int maximumLength() const;

    CuContext *getContext() const;

public slots:
    void setSource(const QString& s);

    void unsetSource();

    void setMaximumLength(int len);

signals:
    void newData(const CuData&);

    void linkStatsRequest(QWidget *myself, CuContextI *myself_as_cwi);

protected:
    void contextMenuEvent(QContextMenuEvent* e);

private:
    QuLabelPrivate *d;

    void m_init();

    // CuDataListener interface
public:
    void onUpdate(const CuData &d);
};

#endif // QUTLABEL_H
