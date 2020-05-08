#ifndef CUMBIAWSWORLD_H
#define CUMBIAWSWORLD_H

#include <vector>
#include <string>

class QJsonDocument;
class CuData;

class CumbiaWSWorld
{
public:
    CumbiaWSWorld();

    void setSrcPatterns(const std::vector<std::string> &pat_regex);

    std::vector<std::string> srcPatterns() const;

    bool source_valid(const std::string& s);

    bool json_decode(const QJsonDocument &json, CuData& out );

private:
    std::vector<std::string> m_src_patterns;
};

#endif // CUMBIAWSWORLD_H
