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

/*! \brief QuComboBox is a writer that uses QComboBox to display values.
 * When the function is activated explicitly, it can write on its target either the displayed value or the index
 *
 * Connection is initiated with setTarget. 
 * The *values* property, if available from the underlying engine, is used to initialize the items at configuration time.
 * 
 * getContext returns a pointer to the CuContext used as a delegate for the connection.
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

    /** \brief returns the target of the writer
     *
     * @return a QString with the name of the target
     */
    QString target() const;

    /** \brief returns a pointer to the CuContext used as a delegate for the connection.
     *
     * @return CuContext
     */
    CuContext *getContext() const;

    bool indexMode() const;

    bool executeOnIndexChanged() const;

    QString getData() const;

public slots:

    /** \brief set the target
     *
     * @param the name of the target
     */
    void setTarget(const QString& target);

    void write(int i);

    void write(const QString& s);

    void setIndexMode(bool im);

    void setExecuteOnIndexChanged(bool exe);

    // ---------------- end of write slots -----------------------

signals:
    void newData(const CuData&);

    void linkStatsRequest(QWidget *myself, CuContextI *myself_as_cwi);

protected:
    void contextMenuEvent(QContextMenuEvent* e);

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
