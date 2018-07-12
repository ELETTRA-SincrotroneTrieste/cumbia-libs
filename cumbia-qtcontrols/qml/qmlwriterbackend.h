#ifndef QmlWriterBackend_H
#define QmlWriterBackend_H

#include <QObject>
#include <cudatalistener.h>
#include <cucontexti.h>
#include <cudata.h>
#include <QVariant>
#include <QString>

class QmlWriterBackendPrivate;
class CuControlsFactoryPool;
class CuContext;
class CumbiaPool_O;

/*! \brief QmlWriterBackend is an object that can be used as a backend for QMLwriters
 *
 * 
 *
*/
class QmlWriterBackend : public QObject, public CuDataListener, public CuContextI
{
    Q_OBJECT
    Q_PROPERTY(bool ok READ ok NOTIFY okChanged)
    Q_PROPERTY(QString target READ target WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(QVariant min READ min NOTIFY minChanged)
    Q_PROPERTY(QVariant max READ max NOTIFY maxChanged)
    Q_PROPERTY(QVariant value READ value NOTIFY valueChanged)
    Q_PROPERTY(QString label READ label NOTIFY labelChanged)
    Q_PROPERTY(QString unit READ unit NOTIFY unitChanged)
    Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)

public:
    QmlWriterBackend(QObject *parent = nullptr);

    virtual ~QmlWriterBackend();

    Q_INVOKABLE void init(CumbiaPool_O *poo_o);


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

    QVariant min() const;

    QVariant max() const;

    QVariant value() const;

    QString label() const;

    QString unit() const;

    QString description() const;

    bool ok() const;

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

    void write(QVariant i);

//    void write(double d);

//    void write(const QString& s);

//    void write(bool b);

//    void write(const QStringList& sl);

//    void write();

    // ---------------- end of write slots -----------------------


signals:
    void newData(const CuData&);

    void labelChanged();

    void minChanged();

    void maxChanged();

    void valueChanged();

    void unitChanged();

    void targetChanged();

    void descriptionChanged();

    void okChanged();


protected:

private:
    QmlWriterBackendPrivate *d;

    void m_init();

    void m_configure(const CuData& d);

    void m_write(const CuVariant& v);


    // CuDataListener interface
public:
    void onUpdate(const CuData &d);
};

#endif
