#ifndef $MAINCLASS$_H
#define $MAINCLASS$_H

#include <$SUPER_INCLUDE$>
#include <cudatalistener.h>
#include <cucontexti.h>
#include <cudata.h>

#include <QString>
class QContextMenuEvent;

class $MAINCLASS$Private;
class Cumbia;
class CumbiaPool;
class CuControlsWriterFactoryI;
class CuControlsFactoryPool;
class CuContext;
class CuLinkStats;

/** \brief $MAINCLASS$ is a reader that uses $SUPERCLASS$ to display values.
 *
 * // startdesc
 * Connection is initiated with setSource. When new data arrives, it is displayed and the newData convenience
 * signal is emitted.
 *
 * getContext returns a pointer to the CuContext used as a delegate for the connection.
 * // enddesc
 *
 */
class $MAINCLASS$ : public $SUPERCLASS$, public CuDataListener, public CuContextI
{
    Q_OBJECT
    Q_PROPERTY(QString target READ target WRITE setTarget DESIGNABLE true)

public:
    $MAINCLASS$(QWidget *w, Cumbia *cumbia, const CuControlsWriterFactoryI &r_fac);

    $MAINCLASS$(QWidget *w, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool);

    virtual ~$MAINCLASS$();

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

public slots:

    /** \brief set the target
     *
     * @param the name of the target
     */
    void setTarget(const QString& target);

    // some examples of slots to write on the target
    //
    // ----------------------------------------------------------
    //
    // *** remove the ones that are not needed by your object ***

    void write(int i);

    void write(double d);

    void write(const QString& s);

    void write(bool b);

    void write(const QStringList& sl);

    void write();

    // ---------------- end of write slots -----------------------

signals:
    void newData(const CuData&);

    void linkStatsRequest(QWidget *myself, CuContextI *myself_as_cwi);

protected:
    void contextMenuEvent(QContextMenuEvent* e);

private:
    $MAINCLASS$Private *d;

    void m_init();

    void m_configure(const CuData& d);

    void m_write(const CuVariant& v);

    void m_set_write_value(const CuVariant& val);

    // utility function that tries to write the property with the given name on this object
    // returns the index of the written property if the operation is successful, -1 otherwise
    // or if the property does not exist
    int m_try_write_property(const QString& propnam, const CuVariant& val);


    // CuDataListener interface
public:
    void onUpdate(const CuData &d);
};

#endif // QUTLABEL_H
