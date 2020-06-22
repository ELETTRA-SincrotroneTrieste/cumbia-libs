#ifndef CUMBIAHTTPWORLD_H
#define CUMBIAHTTPWORLD_H

#include <vector>
#include <string>
#include <QNetworkRequest>

class QJsonDocument;
class CuData;

class CumbiaHTTPWorld
{
public:
    CumbiaHTTPWorld();

    void setSrcPatterns(const std::vector<std::string> &pat_regex);

    std::vector<std::string> srcPatterns() const;

    bool source_valid(const std::string& s);

    bool json_decode(const QByteArray &ba, std::list< CuData>& out ) const;

    bool json_decode(const QJsonValue &v, CuData& out ) const;

    QJsonObject make_error(const QString& msg) const;

private:
    std::vector<std::string> m_src_patterns;

    void m_json_decode(const QJsonValue &o, CuData& out) const;
};

#endif // CUMBIAHTTPWORLD_H
