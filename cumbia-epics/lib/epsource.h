#ifndef EP_SOURCE_H
#define EP_SOURCE_H

#include <string>
#include <list>
#include <cuvariant.h>

using namespace std;

class EpSource
{
public:
    enum Type { PV = 0, Field };

    EpSource();
    EpSource(const std::string s);
    EpSource(const EpSource& other);

    string getPV() const;
    string getName() const;

    std::vector<string> getArgs() const;

    string toString() const;

    Type getType() const;

    EpSource & operator=(const EpSource& other);

    bool operator ==(const EpSource &other) const;

    std::string getArgsString() const;

private:
    std::string m_s;
};

#endif // TSOURCE_H
