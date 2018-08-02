#include "cumbiarndworld.h"

CumbiaRNDWorld::CumbiaRNDWorld()
{
    m_src_patterns.push_back("random.+");
}

void CumbiaRNDWorld::setSrcPatterns(const std::vector<std::string> &pat_regex)
{
    m_src_patterns = pat_regex;
}

std::vector<std::string> CumbiaRNDWorld::srcPatterns() const
{
    return m_src_patterns;
}

bool CumbiaRNDWorld::source_valid(const std::string &s)
{
    return true;
}
