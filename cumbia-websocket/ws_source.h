#ifndef WSSOURCE_H
#define WSSOURCE_H

#include <string>
#include <list>
#include <cuvariant.h>

using namespace std;

class WSSource
{
public:
    enum Type { Cmd = 0, Attr };

    WSSource();
    WSSource(const std::string s);
    WSSource(const WSSource& other);

    string getDeviceName() const;
    string getPoint() const;
    string getName() const;

    std::vector<string> getArgs() const;

    string toString() const;

    Type getType() const;

    WSSource & operator=(const WSSource& other);

    bool operator ==(const WSSource &other) const;

    std::string getArgsString() const;

private:
    string m_s;
};

#endif // TSOURCE_H
