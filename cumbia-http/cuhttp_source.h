#ifndef HTTPSOURCE_H
#define HTTPSOURCE_H

#include <string>
#include <list>
#include <cuvariant.h>

using namespace std;

class CuHTTPSrc
{
public:
    CuHTTPSrc();
    CuHTTPSrc(const std::string& src);
    CuHTTPSrc(const CuHTTPSrc& other);

    string getName() const;
    string getNameNoArgs() const;
    string getProtocol() const;

    std::vector<string> getArgs() const;

    string toString() const;

    CuHTTPSrc & operator=(const CuHTTPSrc& other);

    bool operator ==(const CuHTTPSrc &other) const;

    std::string getArgsString() const;

    bool isSingleShot() const;

private:
    string m_s;
};

#endif // TSOURCE_H
