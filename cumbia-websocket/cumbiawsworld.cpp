#include "cumbiawsworld.h"

CumbiaWSWorld::CumbiaWSWorld()
{
    m_src_patterns.push_back("ws[s]{0,1}://.+");
}

void CumbiaWSWorld::setSrcPatterns(const std::vector<std::string> &pat_regex)
{
    m_src_patterns = pat_regex;
}

std::vector<std::string> CumbiaWSWorld::srcPatterns() const
{
    return m_src_patterns;
}

bool CumbiaWSWorld::source_valid(const std::string &)
{
    return true;
}
