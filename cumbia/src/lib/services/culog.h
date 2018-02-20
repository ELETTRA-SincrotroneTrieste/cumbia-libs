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
 * When a log messge is written with CuLog::write, CuLogImplI::write is called for
 * every registered log implementation.
 * This allows to have a *log implementation* that writes on the console, another
 * one writing on a file and a third one inserting messages on a database.
 *
 * The CuLog::getName and CuLog::getType methods are implemented from the CuServiceI
 * interface for cumbia *services* to identify the specific CuServiceI.
 *
 * @see CuLogImplI
 *
 * @implements CuServiceI
 */
class CuLog : public CuServiceI
{
public:

    enum Class { Generic = 0, Connect, Write, Read, User = 100 };

    enum Level { Info = 0, Warn, Error };

    CuLog(CuLogImplI *log_impl);

    CuLog();

    virtual ~CuLog();

    void addImpl(CuLogImplI *impl);

    void write(const std::__cxx11::string &origin, const std::__cxx11::string &msg, Level l = Error, Class c = Generic);

    void write(const std::string& origin, Level l, Class c, const char *fmt, ...);

    CuLogImplI *getImpl(const std::string &name);

    // CuServiceI interface
public:
    std::string getName() const;
    CuServices::Type getType() const;

private:
    std::list<CuLogImplI *>mLogImpls;
};

class CuLogImplI
{
public:

    virtual ~CuLogImplI() {}

    virtual void write(const std::string & origin, const std::string & msg, CuLog::Level l = CuLog::Error, CuLog::Class c = CuLog::Generic) = 0;

    virtual void write(const std::string& origin, CuLog::Level l, CuLog::Class c, const char *, ...) = 0;

    virtual std::string getName() const = 0;
};

class CuNullLogImpl : CuLogImplI
{
public:
    virtual void write(const std::string &, const std::string &, CuLog::Level = CuLog::Error, CuLog::Class  = CuLog::Generic) {}

    virtual void write(const std::string&, CuLog::Level, CuLog::Class, const char *, ...) {}

    virtual std::string getName() const;
};

class CuConLogImpl : public CuLogImplI
{


    // CuLogImplI interface
public:
    virtual void write(const std::string & origin, const std::string & msg, CuLog::Level l = CuLog::Error, CuLog::Class c = CuLog::Generic);

    virtual void write(const std::string& origin, CuLog::Level l, CuLog::Class c, const char *, ...);


    virtual std::string getName() const;
};


#endif // CULOG_H
