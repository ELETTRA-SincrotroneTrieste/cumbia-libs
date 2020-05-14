#ifndef CUHTTP_PROTOCOLHELPERS_H
#define CUHTTP_PROTOCOLHELPERS_H

class ProtocolHelper_I;

#include <QString>
#include <QMap>

class CuHttpProtocolHelpers
{
public:
    CuHttpProtocolHelpers();

    ~CuHttpProtocolHelpers();

    ProtocolHelper_I *get(const QString& protonam) const;

private:
    QMap<QString, ProtocolHelper_I *> m_helpersMap;
};

#endif // CUHTTP_PROTOCOLHELPERS_H
