#include "culog.h"
#include <stdio.h>
#include <cstdarg>
#include <cumacros.h>

CuLog::CuLog(CuLogImplI *log_impl)
{
    std::list<CuLogImplI *>::iterator it = mLogImpls.begin();
    mLogImpls.insert(it, log_impl);
}

CuLog::CuLog()
{

}

CuLog::~CuLog()
{
    pdelete("~CuLog %p", this);
}

void CuLog::addImpl(CuLogImplI *impl)
{
    std::list<CuLogImplI *>::iterator it = mLogImpls.begin();
    mLogImpls.insert(it, impl);
}

void CuLog::write(const std::string & origin, const std::string & msg, CuLog::Level l, CuLog::Class c)
{
    std::list<CuLogImplI *>::iterator it;
    for(it = mLogImpls.begin(); it != mLogImpls.end(); ++it)
         (*it)->write(origin, msg, l, c);
}

void CuLog::write(const std::string& origin, CuLog::Level l, CuLog::Class c, const char *fmt, ...)
{
    va_list s;
    va_start( s, fmt);
    std::list<CuLogImplI *>::iterator it;
    for(it = mLogImpls.begin(); it != mLogImpls.end(); ++it)
        (*it)->write(origin, l, c, fmt, s);
    va_end(s);
}

CuLogImplI *CuLog::getImpl(const std::__cxx11::string &name)
{
    std::list<CuLogImplI *>::iterator it;
    for(it = mLogImpls.begin(); it != mLogImpls.end(); ++it)
        if((*it)->getName() == name)
            return (*it);
    return NULL;
}

std::__cxx11::string CuLog::getName() const
{
    return "CuLog";
}

CuServices::Type CuLog::getType() const
{
    return CuServices::Log;
}

void CuConLogImpl::write(const std::string & origin, const std::string & msg, CuLog::Level l, CuLog::Class c)
{
    if(l == CuLog::Info)
        fprintf(stdout, "\e[1;36mi\e[0m: \e[3m%s\e[0m: %s\n", origin.c_str(), msg.c_str());
    else if(l == CuLog::Warn)
        fprintf(stdout, "\e[1;33mw\e[0m: \e[3m%s\e[0m: %s\n", origin.c_str(), msg.c_str());
    else
        fprintf(stderr, "\e[1;31me\e[0m: \e[0;31;3m%s\e[0m: %s\n", origin.c_str(), msg.c_str());

}

void CuConLogImpl::write(const std::__cxx11::string &origin, CuLog::Level l, CuLog::Class c,  const char *fmt, ...)
{
    va_list s;
    va_start( s, fmt);
    if(l == CuLog::Info)
        fprintf(stdout, "\e[1;36mi\e[0m: \e[3m%s\e[0m: ", origin.c_str());
    else if(l == CuLog::Warn)
        fprintf(stdout, "\e[1;33mw\e[0m: \e[3m%s\e[0m: ", origin.c_str());
    else
        fprintf(stderr, "\e[1;31;4merror\e[0m: \e[0;31;3m%s\e[0m: ", origin.c_str());

    vfprintf(stdout, fmt, s);
    fprintf(stderr, "\n");
    va_end(s);
}


std::__cxx11::string CuConLogImpl::getName() const
{
    return std::string("ConsoleLogImpl");
}


std::__cxx11::string CuNullLogImpl::getName() const { return std::string("NullLogImpl"); }
