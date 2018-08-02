#ifndef RNDSOURCE_H
#define RNDSOURCE_H

#include <string>
#include <list>
#include <cuvariant.h>

using namespace std;

class RNDSource
{
public:
    enum Type { Cmd = 0, Attr };

    RNDSource();
    RNDSource(const std::string s);
    RNDSource(const RNDSource& other);

    string getDeviceName() const;
    string getPoint() const;
    string getName() const;

    std::vector<string> getArgs() const;

    string toString() const;

    Type getType() const;

    RNDSource & operator=(const RNDSource& other);

    bool operator ==(const RNDSource &other) const;

    std::string getArgsString() const;

private:
    string m_s;
};

#endif // TSOURCE_H
