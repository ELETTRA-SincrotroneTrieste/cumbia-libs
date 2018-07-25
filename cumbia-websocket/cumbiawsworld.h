#ifndef CUMBIAWSWORLD_H
#define CUMBIAWSWORLD_H

#include <vector>
#include <string>


class CumbiaWSWorld
{
public:
    CumbiaWSWorld();

    void setSrcPatterns(const std::vector<std::string> &pat_regex);

    std::vector<std::string> srcPatterns() const;

    bool source_valid(const std::string& s);

private:
    std::vector<std::string> m_src_patterns;
};

#endif // CUMBIAWSWORLD_H
