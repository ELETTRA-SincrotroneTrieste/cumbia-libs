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

    bool json_decode(const QJsonDocument &json, CuData& out );

private:
    std::vector<std::string> m_src_patterns;
};

#endif // CUMBIAHTTPWORLD_H
