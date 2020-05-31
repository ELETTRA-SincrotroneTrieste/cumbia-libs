#ifndef CULOG_H
#define CULOG_H

#include <stdarg.h>
#include <cuservicei.h>
#include <list>
#include <string>

class CuLogImplI;

/*! \brief a cumbia service providing an interface for logging facilities
 *
 * The class is designed to provide a logging facility with the following features:
 * \li define three levels: Info, Warn, Error
 * \li define a "*class*" of log: Generic, Connect, Write, Read, *user defined*
 *
 * Several *log implementations* (CuLogImpl) can be added to CuLog  through the
 * CuLog::addImpl method. A pointer to a registered *log implementation* can be
 * obtained from CuLog::getImpl specifying a name as string. CuLogImplI interface
 * actually defines a CuLogImplI::getName method to force the specific log
 * implementation to provide a name
 *
 * When a log message is written with CuLog::write, CuLogImplI::write is called for
 * every registered log implementation.
 * This allows to have a *log implementation* that writes on the console, another
 * one writing on a file and a third one inserting messages on a database.
 *
 * The CuLog::getName and CuLog::getType methods are implemented from the CuServiceI
 * interface for cumbia *services* to identify the specific CuServiceI.
 *
 * This service can enable, disable, change the
 * logging method at runtime. This can be useful for test purposes.
 * CuNullLogImpl is a *null* implementation of CuLogImplI which *write* methods are
 * empty.
 * CuConLogImpl is a CuLogImplI that prints messages on the console.
 *
 * @see CuLogImplI
 *
 * @implements CuServiceI
 */
class CuLog : public CuServiceI
{
public:

    enum Category { Generic = 0, Network, Write, Read, User = 100 };

    enum Level { Debug = 0, Info, Warn, Error };

    CuLog(CuLogImplI *log_impl);

    CuLog();

    virtual ~CuLog();

    void addImpl(CuLogImplI *impl);

    void removeImpl(const std::string& name);

    void write(const std::string &origin, const std::string &msg, int l = Error, int c = Generic);

    void write(const std::string& origin, int l, const char *fmt, ...);

    void write(const std::string& origin, int l, int category, const char *fmt, ...);

    CuLogImplI *getImpl(const std::string &name);

    // CuServiceI interface
public:
    std::string getName() const;
    CuServices::Type getType() const;

private:
    std::list<CuLogImplI *>mLogImpls;
};

/*! \brief interface for a concrete log implementation
 *
 * This interface defines two write methods to write a message on a logging system
 * (console, file, database, null...) and a getName method that compels every subclass
 * to provide a unique name
 *
 * The calls to CuLog::write are forwarded to every registered CuLogImplI implementation
 *
 * See the CuLog::write documentation for details and CuConLogImpl::write for an example
 * from an implementor.
 *
 */
class CuLogImplI
{
public:

    /*! \brief class destructor
     *
     * virtual destructor
     */
    virtual ~CuLogImplI() {}

    virtual void write(const std::string & origin, const std::string & msg, int l = CuLog::Error, int category = CuLog::Generic) = 0;

    virtual std::string getName() const = 0;
};

/*! \brief null log implementation
 *
 * The write methods of this log implementation have empty bodies.
 * You can replace the currently registered CuLogImplI with this one
 * if you want to stop logging at runtime.
 */
class CuNullLogImpl : CuLogImplI
{
public:
    virtual void write(const std::string &, const std::string &, int  = CuLog::Error, int   = CuLog::Generic) {}

    virtual std::string getName() const;
};

/*! \brief console log implementation
 *
 * This *logging method* prints messages on the console
 *
 * See the CuLog::write documentation for a description of the *write* methods
 */
class CuConLogImpl : public CuLogImplI
{
    // CuLogImplI interface
public:
    virtual void write(const std::string & origin, const std::string & msg, int l = CuLog::Error, int c = CuLog::Generic);

    virtual std::string getName() const;
};


#endif // CULOG_H
