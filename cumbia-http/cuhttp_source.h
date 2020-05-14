#ifndef HTTPSOURCE_H
#define HTTPSOURCE_H

#include <string>
#include <list>
#include <cuvariant.h>

using namespace std;

class HTTPSource
{
public:
    HTTPSource();
    HTTPSource(const std::string& src);
    HTTPSource(const HTTPSource& other);

    string getName() const;
    string getFullName() const;
    string getProtocol() const;

    std::vector<string> getArgs() const;

    string toString() const;

    HTTPSource & operator=(const HTTPSource& other);

    bool operator ==(const HTTPSource &other) const;

    std::string getArgsString() const;

private:
    string m_s;
};

#endif // TSOURCE_H
