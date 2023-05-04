#ifndef QULABEL_H
#define QULABEL_H

#include <cudatalistener.h>
#include <cucontexti.h>
#include <cudata.h>
#include <qupalette.h>
#include "qulabelbase.h"

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
 * Connection is initiated with setSource. When new data arrives, it is displayed and the newData
 * signal is emitted. A propertyReady signal is provided for convenience and emitted only once at
 * configuration time.
 *
 * \par Display enumeration values (if supported by the engine)
 * If the underlying engine supports a configuration property named *values*, the list of strings
 * is used to associate a value to a label.
 * Likewise, if the engine supports a *colors* property, the list of colors is used to associate
 * a value to a color.
 * The *values* and *colors* properties, if available, are extracted at configuration time into a
 * *std::vector* of *std::string*. The value of the source is used as *index* to pick up the label
 * and the color from the vectors. *Indexes* start from zero.
 * Labels are displayed as they are, while color strings are mapped through
 * QuPalette into QColor. The QuPalette used is accessible through quPalette and setQuPalette.
 *
 * getContext returns a pointer to the CuContext used as a delegate for the connection.
 *
 */
class QuLabel : public QuLabelBase, public CuDataListener, public CuContextI
{
    Q_OBJECT
    Q_PROPERTY(QString source READ source WRITE setSource DESIGNABLE true)
    Q_PROPERTY(bool displayUnitEnabled READ displayUnitEnabled WRITE setDisplayUnitEnabled DESIGNABLE true)

public:
    QuLabel(QWidget *w, Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac);

    QuLabel(QWidget *w, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool);

    virtual ~QuLabel();

    QString source() const;
    CuContext *getContext() const;

    bool displayUnitEnabled() const;
    QString displayUnit() const;

    QuPalette quPalette() const;
    void setQuPalette(const QuPalette &colors);

public slots:
    void setSource(const QString& s);
    void unsetSource();

    void setDisplayUnitEnabled(bool en);
    virtual bool ctxSwap(CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool);

signals:
    void newData(const CuData&);

    void propertyReady(const CuData& );

    void linkStatsRequest(QWidget *myself, CuContextI *myself_as_cwi);

protected:
    void contextMenuEvent(QContextMenuEvent* e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);

private:
    QuLabelPrivate *d;

    void m_init();

    void m_initCtx();

    void m_configure(const CuData& da);

    // CuDataListener interface
public:
    void onUpdate(const CuData &d);
};

#endif // QUTLABEL_H
