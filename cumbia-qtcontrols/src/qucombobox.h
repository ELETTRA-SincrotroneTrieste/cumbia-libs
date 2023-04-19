#ifndef QuComboBox_H
#define QuComboBox_H

#include <QComboBox>
#include <cudatalistener.h>
#include <cucontexti.h>
#include <cudata.h>

#include <QString>
class QContextMenuEvent;

class QuComboBoxPrivate;
class Cumbia;
class CumbiaPool;
class CuControlsWriterFactoryI;
class CuControlsFactoryPool;
class CuContext;
class CuLinkStats;

/*! \brief QuComboBox derives from Qt QComboBox to display values.
 * When the option is activated explicitly, it can write either the current text or the current index
 *
 * Connection is initiated with setTarget. 
 * The *values* property, if available from the underlying engine, is used to initialize the items at configuration time.
 * 
 * getContext returns a pointer to the CuContext used as a delegate for the connection.
 *
 * @see indexMode
 * @see executeOnIndexChanged
 *
 * If the object is used to provide input arguments to other writers (e.g. QuPushButton), the data property
 * provides a string representation of either the current text or index, according to indexMode. @see CuControlsUtils
 *
*/
class QuComboBox : public QComboBox, public CuDataListener, public CuContextI
{
    Q_OBJECT
    Q_PROPERTY(QString target READ target WRITE setTarget DESIGNABLE false)
    // the source property is to allow easy qtango project import
    Q_PROPERTY(QString source READ target WRITE setTarget DESIGNABLE false)
    Q_PROPERTY(bool indexMode READ indexMode WRITE setIndexMode DESIGNABLE true)
    Q_PROPERTY(bool executeOnIndexChanged READ executeOnIndexChanged WRITE setExecuteOnIndexChanged DESIGNABLE true)
    Q_PROPERTY(QString data READ getData DESIGNABLE false)

public:
    QuComboBox(QWidget *w, Cumbia *cumbia, const CuControlsWriterFactoryI &r_fac);

    QuComboBox(QWidget *w, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool);

    virtual ~QuComboBox();

    QString target() const;

    CuContext *getContext() const;

    bool indexMode() const;

    bool executeOnIndexChanged() const;

    QString getData() const;

public slots:
    void setTarget(const QString& target);
    void setTarget(const QString& target, CuContext *ctx);
    void clearTarget();

    void write(int i);

    void write(const QString& s);

    void setIndexMode(bool im);

    void setExecuteOnIndexChanged(bool exe);

    void onAnimationValueChanged(const QVariant& v);

signals:
    void newData(const CuData&);

    void linkStatsRequest(QWidget *myself, CuContextI *myself_as_cwi);

protected:
    void contextMenuEvent(QContextMenuEvent* e);
    void paintEvent(QPaintEvent *pe);

private:
    QuComboBoxPrivate *d;

    void m_init();

    void m_configure(const CuData& d);

    void m_write(const CuVariant& v);

    // CuDataListener interface
public:
    void onUpdate(const CuData &d);

private slots:
    void m_onIndexChanged(int i);
};

#endif // QUTLABEL_H
