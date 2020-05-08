#ifndef PROTOCOLHELPERS_H
#define PROTOCOLHELPERS_H

class ProtocolHelper_I;

#include <QString>
#include <QMap>

class CuWsProtocolHelpers
{
public:
    CuWsProtocolHelpers();

    ~CuWsProtocolHelpers();

    ProtocolHelper_I *get(const QString& protonam) const;

private:
    QMap<QString, ProtocolHelper_I *> m_helpersMap;
};

#endif // PROTOCOLHELPERS_H
