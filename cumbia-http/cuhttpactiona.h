#ifndef CUHTTPACTIONA_H
#define CUHTTPACTIONA_H

#include <cuthreadlistener.h>
#include <cuhttp_source.h>
#include <QObject>
#include <QSslError>
#include <QNetworkReply>

class CuDataListener;
class QString;
class QJsonDocument;
class CuHTTPActionAPrivate;
class CuHTTPActionListener;

class CuHTTPActionA : public QObject
{
    Q_OBJECT
public:

    enum Type { Reader = 0, Writer, Config, ChannelReceiver };

    CuHTTPActionA(QNetworkAccessManager *nam);

    /*! \brief virtual destructor, to make compilers happy
     */
    virtual ~CuHTTPActionA();

    /*! \brief return a HTTPSource object that stores information about the Tango source
     *
     * @return HTTPSource, an object storing information on the Tango source
     */
    virtual QString getSourceName() const  = 0;

    /*! \brief returns the type of CuHTTPActionI implemented
     *
     * @return one of the values from the Type enum
     */
    virtual Type getType() const = 0;

    /*! \brief insert a CuDataListener to the list of listeners of data updates
     *
     * @param l a CuDataListener that receives notifications when data is ready,
     *        through CuDataListener::onUpdate
     */
    virtual void addDataListener(CuDataListener *l) = 0;

    /*! \brief remove a CuDataListener from the list of listeners of data updates
     *
     * @param l a CuDataListener that will be removed from the list of listeners
     */
    virtual void removeDataListener(CuDataListener *l) = 0;

    /*! \brief return the number of registered data listeners
     *
     * @return the number of data listeners currently installed
     */
    virtual size_t dataListenersCount() = 0;

    virtual void start() = 0;

    virtual bool exiting() const = 0;

    virtual void stop() = 0;

    virtual void decodeMessage(const QJsonDocument& json) = 0;

    virtual void setHttpActionListener(CuHTTPActionListener *l);

    virtual QNetworkRequest prepareRequest(const QUrl& url) const;

    virtual void notifyActionFinished();

protected slots:
    virtual void onNewData();
    virtual void onReplyFinished();
    virtual void onReplyDestroyed(QObject *);
    virtual void onSslErrors(const QList<QSslError> &errors);
    virtual void onError(QNetworkReply::NetworkError code);

    virtual void startRequest(const QUrl& src);
    virtual void stopRequest();

    CuHTTPActionListener *getHttpActionListener() const ;

    QNetworkAccessManager *getNetworkAccessManager() const;

private:
    CuHTTPActionAPrivate *d;

    void m_on_buf_complete();
    QByteArray m_extract_data(const QByteArray& in) const;

};

/*!
 * \brief The CuHTTPActionListener implementations are notified when an action is started and when it is finished.
 *
 * The only implementation in this library is CumbiaHttp
 */
class CuHTTPActionListener {
public:
    virtual void onActionStarted(const std::string &source, CuHTTPActionA::Type t) = 0;
    virtual void onActionFinished(const std::string &source, CuHTTPActionA::Type t) = 0;
};


#endif // CUHTTPACTIONA_H
