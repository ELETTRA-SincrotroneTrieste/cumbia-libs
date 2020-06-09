#ifndef HTTPSOURCE_H
#define HTTPSOURCE_H

#include <QString>
#include <QList>
#include <cuvariant.h>

#include "cuhttpsrchelper_i.h"

using namespace std;

class CuHTTPSrcPrivate {
public:
    std::string m_s;
    CuHttpSrcHelper_I* helper;
};

class CuHTTPSrc
{
public:
    CuHTTPSrc();
    virtual ~CuHTTPSrc();
    CuHTTPSrc(const std::string &src, const QList<CuHttpSrcHelper_I*>& srchs);
    CuHTTPSrc(const CuHTTPSrc& other);
    std::string getProtocol() const;
    std::string toString() const;
    CuHTTPSrc & operator=(const CuHTTPSrc& other);
    bool operator ==(const CuHTTPSrc &other) const;
    bool canMonitor() const;
    std::string getName() const;

    std::string native_type() const;
    std::string prepare() const;
    std::string get(const char* what) const;

private:
    CuHTTPSrcPrivate d;
    CuHttpSrcHelper_I *m_find_helper(const std::string &src, const QList<CuHttpSrcHelper_I *> &helpers);
};

#endif // TSOURCE_H
