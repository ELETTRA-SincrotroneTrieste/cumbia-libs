#ifndef CULOG_H
#define CULOG_H

#include <stdarg.h>
#include <cuservicei.h>
#include <list>
#include <string>

class CuLogImplI;

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
