#ifndef CUTTHREADTOKENGEN_H
#define CUTTHREADTOKENGEN_H

#include <cuthreadtokengeni.h>
#include <list>
#include <string>

class CuTThreadTokenGenPrivate;

class CuTThreadTokenGen : public CuThreadTokenGenI
{
public:    
    CuTThreadTokenGen(int pool_siz, const std::string& tok_prefix = "tthread_");

    ~CuTThreadTokenGen();

    // CuThreadTokenGenI interface
    CuData generate(const CuData &in);

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
