#ifndef TSOURCE_H
#define TSOURCE_H

#include <string>
#include <list>
#include <cuvariant.h>

using namespace std;

class TSource
{
public:
    enum Type { Cmd = 0, Attr };

    TSource();
    TSource(const std::string s);
    TSource(const TSource& other);

    string getDeviceName() const;
    string getPoint() const;
    string getName() const;

    std::vector<string> getArgs() const;

    string toString() const;

    Type getType() const;

    TSource & operator=(const TSource& other);

    bool operator ==(const TSource &other) const;

    std::string getArgsString() const;

private:
    string m_s;
};

#endif // TSOURCE_H
