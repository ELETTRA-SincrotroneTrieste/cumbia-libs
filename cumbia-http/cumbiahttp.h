#ifndef CUMBIAHTTP_H
#define CUMBIAHTTP_H

#include <cumbia.h>
#include <string>
#include <cuhttpactionreader.h>
#include <cuhttpactionfactoryi.h>
#include <QString>

class CuThreadFactoryImplI;
class CuThreadsEventBridgeFactory_I;
class CuDataListener;
class QuReplaceWildcards_I;

class CumbiaHttpPrivate;
/*!
 * \mainpage Cumbia <em>http</em> module
 *
 * The *cumbia http* module provides an interface to send and receive data through *http* and *https*.
 * The library relies on the *canone3* server (caserver) that has been developed for the
 * *PWMA mobile and web interface*:
 *
 * - The <a href="https://gitlab.com/PWMA/">PWMA</a> project
 *
 * The *cumbia http* module has been developed in order to build applications for <strong>desktop and mobile devices</strong>
 * able to interact with the Tango and EPICS control systems through the aforementioned *canone server*
 * (<a href="https://gitlab.com/PWMA/">PWMA</a>).
 *
 * CumbiaHttp is a CuHTTPActionListener in order to be notified when an action is finished, so that it can
 * be unregistered and deleted.
 *
 */
class CumbiaHttp : public Cumbia, public CuHTTPActionListener
{

public:
    enum Type { CumbiaHTTPType = Cumbia::CumbiaUserType + 18 };

    CumbiaHttp(const QString& url, const QString &channel, CuThreadFactoryImplI *tfi, CuThreadsEventBridgeFactory_I *teb);

    ~CumbiaHttp();

    CuThreadFactoryImplI* getThreadFactoryImpl() const;

    CuThreadsEventBridgeFactory_I* getThreadEventsBridgeFactory() const;

    QString url() const;

    virtual int getType() const;

    void addAction(const std::string &source, CuDataListener *l, const CuHTTPActionFactoryI &f);
    void unlinkListener(const string &source, CuHTTPActionA::Type t, CuDataListener *l);
    CuHTTPActionA *findAction(const std::string &source, CuHTTPActionA::Type t) const;

    void addReplaceWildcardI(QuReplaceWildcards_I *rwi);

    QList<QuReplaceWildcards_I *> getReplaceWildcard_Ifaces() const;

private:

    void m_init();

    CumbiaHttpPrivate *d;


    // CuHTTPActionListener interface
public:
    void onActionStarted(const string &source, CuHTTPActionA::Type t);
    void onActionFinished(const string &source, CuHTTPActionA::Type t);
};

#endif // CUMBIAHTTP_H
