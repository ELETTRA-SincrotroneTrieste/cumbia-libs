#ifndef CUTTHREADTOKENGEN_H
#define CUTTHREADTOKENGEN_H

#include <cuthreadtokengeni.h>
#include <list>
#include <string>

class CuTThreadTokenGenPrivate;

/*!
 * \brief Thread token generator for Tango sources.
 *
 * A *thread token* determines how threads are grouped
 * in Cumbia. When Cumbia::registerActivity is called, the thread token will assign an activity
 * to the thread with the given token. Activities with the *same token* are executed in the *same thread*.
 *
 * Tokens for grouping Tango sources can be either generated automatically, with an imposed limit to the set,
 * or through a *user defined map*. When the limit is reached, the first method starts reusing threads, ensuring at the
 * same time that sources referring to the same devices belong to the same thread. The automatic mode is
 * employed if no user map has been defined.
 * The CuTThreadTokenGen::generate method provides more details.
 *
 * Use setThreadPoolSize to fix a limit on the number of desired threads.
 *
 * After instantiating and configuring CuTThreadTokenGen, call Cumbia::setThreadTokenGenerator
 * from your application to use the token generator.
 *
 * \par Recommended readings
 * \li CuThreadTokenGenI
 * \li Cumbia::setThreadTokenGenerator
 * \li Cumbia::threadToken
 */
class CuTThreadTokenGen : public CuThreadTokenGenI
{
public:    
    CuTThreadTokenGen(int pool_siz, const std::string& tok_prefix = "tthread_");

    ~CuTThreadTokenGen();

    // CuThreadTokenGenI interface
    std::string generate(const std::string& dev);

    void map(const std::string &device, int which);

    int which_thread(const std::string &device) const;

    int which_from_token(const std::string& tok) const;

    std::list<std::string> devicesForToken(const std::string& token) const;

    std::string tok_prefix() const;

    int threadPoolSize() const;

    void setThreadPoolSize(int siz);

private:
    CuTThreadTokenGenPrivate *d;
};

#endif // CUTTHREADTOKENGEN_H
