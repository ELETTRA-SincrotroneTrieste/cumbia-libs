#include "culog.h"
#include <stdio.h>
#include <cstdarg>
#include <cumacros.h>

/*! \brief class constructor accepting a CuLogImplI pointer for initialization
 *
 * @param log_impl an instance of a CuLogImplI implementation that is immediately
 *        added and used
 *
 * See also CuLog::addImpl
 */
CuLog::CuLog(CuLogImplI *log_impl)
{
    std::list<CuLogImplI *>::iterator it = mLogImpls.begin();
    mLogImpls.insert(it, log_impl);
}

/*! \brief empty class constructor
 *
 * Class constractor with no parameters
 */
CuLog::CuLog()
{

}

/*! \brief the class destructor
 *
 * virtual class destructor
 *
 * \par note
 * Does not delete implementations
 */
CuLog::~CuLog()
{
    pdelete("~CuLog %p", this);
}

/*! \brief register a new CuLogImplI implementation into the logging service
 *
 * @param impl an instance of a CuLogImplI implementation, such as CuConLogImpl or
 *        QuLogImpl from cumbia-qtcontrols module
 *
 * When CuLog::write is invoked, the call is forwarded to every registered implementation
 * of CuLogImplI
 */
void CuLog::addImpl(CuLogImplI *impl)
{
    std::list<CuLogImplI *>::iterator it = mLogImpls.begin();
    mLogImpls.insert(it, impl);
}

/*! \brief remove the log implementation with the given name from the logging service
 *
 * @param name a string with the *log implementation*'s name to remove from the service
 *
 * A *log implementation* can be removed at runtime in order to disable logging.
 */
void CuLog::removeImpl(const std::string &name)
{
    CuLogImplI *li = getImpl(name);
    if(li)
        mLogImpls.remove(li);
}

/*! \brief writes a log message on every CuLogImplI registered with addImpl
 *
 * @param origin the source of the message
 * @param msg the message
 * @param l the log *level*, one of CuLog::Level enum values
 * @param c the log *class*, one of CuLog::Class enum values
 *
 * Calls the CuLogImplI::write(const std::string & origin, const std::string & msg, CuLog::Level l = CuLog::Error, CuLog::Class c = CuLog::Generic)
 * on all registered CuLogImplI's
 */
void CuLog::write(const std::string & origin, const std::string & msg, CuLog::Level l, CuLog::Class c)
{
    std::list<CuLogImplI *>::iterator it;
    for(it = mLogImpls.begin(); it != mLogImpls.end(); ++it)
         (*it)->write(origin, msg, l, c);
}

/*! \brief writes a log message on every CuLogImplI registered with addImpl (with printf format)
 *
 * @param origin the source of the message
 * @param l the log *level*, one of CuLog::Level enum values
 * @param c the log *class*, one of CuLog::Class enum values
 * @param fmt the format ... á la printf, using va_list
 *
 * Calls the CuLogImplI::write(const std::string &, CuLog::Level, CuLog::Class, const char* fmt, ...)
 * on all registered CuLogImplI's
 *
 * \par example
 * \code
 * log.write("CuTReader.stop", CuLog::Error, CuLog::Read,
 *      "stop called twice for reader %s", data["source"].toString().c_str());
 * \endcode
 */
void CuLog::write(const std::string& origin, CuLog::Level l, CuLog::Class c, const char *fmt, ...)
{
    va_list s;
    va_start( s, fmt);
    std::list<CuLogImplI *>::iterator it;
    for(it = mLogImpls.begin(); it != mLogImpls.end(); ++it)
        (*it)->write(origin, l, c, fmt, s);
    va_end(s);
}

/*!
 * \brief returns the CuLogImplI implementation with the given name
 * \param name the name of the *log implementation* you are requesting
 * \return the CuLogImplI implementation with the given name or null if no
 *         *log implementation* bearing that name was registered.
 *
 * Every CuLogImplI subclass must implement a CuLogImplI::getName method
 * returning an identifying name for the class.
 */
CuLogImplI *CuLog::getImpl(const std::string &name)
{
    std::list<CuLogImplI *>::iterator it;
    for(it = mLogImpls.begin(); it != mLogImpls.end(); ++it)
        if((*it)->getName() == name)
            return (*it);
    return NULL;
}

/*!
 * \brief returns the name of the *cumbia service*
 *
 * \return the service name, "CuLog"
 *
 * @see CuServiceI::getName
 * @see CuServiceProvider::registerService
 * @see CuServiceProvider::get
 *
 * @implements CuServiceI::getName
 */
std::string CuLog::getName() const
{
    return "CuLog";
}

/*!
 * \brief returns the type of the *cumbia service*
 *
 * \return the service type, CuServices::Log
 *
 * @see CuServiceI::getType
 * @see CuServiceProvider::registerService
 * @see CuServiceProvider::get
 *
 * @implements CuServiceI::getName
 */
CuServices::Type CuLog::getType() const
{
    return CuServices::Log;
}

/*! \brief writes a message on the console.
 *
 * Writes on stderr if the CuLog::Level is different from CuLog::Info and CuLog::Warn,
 * on stdout otherwise
 *
 * See CuLog::write(const std::string & origin, const std::string & msg, CuLog::Level l, CuLog::Class c)
 * for the input parameters description
 */
void CuConLogImpl::write(const std::string & origin, const std::string & msg, CuLog::Level l, CuLog::Class c)
{
    if(l == CuLog::Info)
        fprintf(stdout, "\e[1;36mi\e[0m: \e[3m%s\e[0m: %s\n", origin.c_str(), msg.c_str());
    else if(l == CuLog::Warn)
        fprintf(stdout, "\e[1;33mw\e[0m: \e[3m%s\e[0m: %s\n", origin.c_str(), msg.c_str());
    else
        fprintf(stderr, "\e[1;31me\e[0m: \e[0;31;3m%s\e[0m: %s\n", origin.c_str(), msg.c_str());

}

/*! \brief writes a message on the console.
 *
 * Writes on stderr if the CuLog::Level is different from CuLog::Info and CuLog::Warn,
 * on stdout otherwise
 *
 * See CuLog::write(const std::string& origin, CuLog::Level l, CuLog::Class c, const char *fmt, ...)
 * for the input parameters description
 */
void CuConLogImpl::write(const std::string &origin, CuLog::Level l, CuLog::Class c,  const char *fmt, ...)
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

/*! \brief returns the name of the console log implementation
 *
 * @return returns the string "ConsoleLogImpl"
 *
 * @implements CuLogImplI::getName
 */
std::string CuConLogImpl::getName() const
{
    return std::string("ConsoleLogImpl");
}

/*! \brief returns the name of the *null* implementation
 *
 * @return the const string "NullLogImpl"
 *
 * @implements CuLogImplI::getName
 */
std::string CuNullLogImpl::getName() const { return std::string("NullLogImpl"); }
