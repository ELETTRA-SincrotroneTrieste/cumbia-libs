#ifndef BotReader_H
#define BotReader_H

#include <QObject>
#include <cudatalistener.h>
#include <cucontexti.h>
#include <cudata.h>

#include <QString>
class QContextMenuEvent;

class BotReaderPrivate;
class Cumbia;
class CumbiaPool;
class CuControlsReaderFactoryI;
class CuControlsFactoryPool;
class CuContext;
class CuLinkStats;

/*! \brief BotReader is a reader that uses QObject to display values.
 *
 * Connection is initiated with setSource. When new data arrives, it is displayed and the newData convenience
 * signal is emitted.
 * 
 * getContext returns a pointer to the CuContext used as a delegate for the connection.
 *
*/
class BotReader : public QObject, public CuDataListener, public CuContextI
{
    Q_OBJECT
    Q_PROPERTY(QString source READ source WRITE setSource DESIGNABLE true)

public:

    // message delivered silently or not
    enum Priority { Normal, Low };

    BotReader(int user_id,
              int chat_id, QObject *w,
              CumbiaPool *cumbia_pool,
              const CuControlsFactoryPool &fpool,
              const QString& formula,
              Priority pri = Normal,
              const QString& host = QString(),
              bool monitor = false);

    virtual ~BotReader();

    void setPropertyEnabled(bool get_props);

    /** \brief returns the source of the reader
     *
     * @return a QString with the name of the source
     */
    QString source() const;

    QString formula() const;

    QString host() const;

    /** \brief returns a pointer to the CuContext used as a delegate for the connection.
     *
     * @return CuContext
     */
    CuContext *getContext() const;

    int userId() const;

    Priority priority() const;

    void setPriority(Priority pri);

public slots:

    /** \brief set the source and start reading
     *
     * @param the name of the source
     */
    void setSource(const QString& s);

    /** \brief disconnect the source
     *
     * remove the source and stop reading
     */
    void unsetSource();

    void setFormula(const QString& formula);

signals:
    void newData(int chat_id, const CuData&);
    void startSuccess(int user_id, int chat_it, const QString& src, const QString& formula);
    void onProperties(int chat_id, const CuData&);
    void formulaChanged(int chat_id, const QString& src, const QString& oldf, const QString& new_f);
    void priorityChanged(int chat_id, const QString& src, BotReader::Priority oldpri, BotReader::Priority newpri);

protected:


private:
    BotReaderPrivate *d;

    void m_init();

    void m_configure(const CuData& d);

    bool m_publishResult(const CuData& da);

    // CuDataListener interface
public:
    void onUpdate(const CuData &d);
};

#endif // QUTLABEL_H
