#include "cutangoopt_builder.h"
#include <cuvariant.h>

CuTangoOptBuilder::CuTangoOptBuilder()
{
    m_options.add("refresh_mode", CuTReader::ChangeEventRefresh);
    m_options.add("fetch_history", false);
    m_options.add("period", 1000);
    m_options.add("fetch_props", std::vector<std::string>());
}

CuTangoOptBuilder::CuTangoOptBuilder(int per, CuTReader::RefreshMode mod)
{
    m_options.add("period",  per);
    m_options.add("refresh_mode" , mod);
    m_options.add("fetch_history", false);
    m_options.add("fetch_props", std::vector<std::string>());
}

CuTangoOptBuilder::CuTangoOptBuilder(const CuData &d)
{
    m_options = d;
}

CuTangoOptBuilder::CuTangoOptBuilder(const CuTangoOptBuilder &other)
{
    m_options = other.m_options;
}

CuTangoOptBuilder &CuTangoOptBuilder::setFetchAttProps(const std::vector<string> &props)
{
    m_options.add("fetch_props", props);
    return *this;
}

CuTangoOptBuilder & CuTangoOptBuilder::addFetchAttProp(const string &s)
{
    std::vector<std::string> prps = m_options.value("fetch_props").toStringVector();
    prps.push_back(s);
    m_options.add("fetch_props", prps);
    return *this;
}

CuTangoOptBuilder & CuTangoOptBuilder::setFetchAttHistory(bool fetch)
{
    m_options.add("fetch_history", fetch);
    return *this;
}

CuTangoOptBuilder & CuTangoOptBuilder::setPeriod(int millis)
{
    m_options.add("period", millis);
    return *this;
}

CuTangoOptBuilder & CuTangoOptBuilder::setRefreshMode(CuTReader::RefreshMode mode)
{
    m_options.add("refresh_mode", mode);
    return *this;
}

bool CuTangoOptBuilder::operator ==(const CuTangoOptBuilder &other) const
{
    return m_options == other.m_options;
}

int CuTangoOptBuilder::period() const
{
    return m_options.value("period").toInt();
}

CuTReader::RefreshMode CuTangoOptBuilder::mode() const
{
    return static_cast<CuTReader::RefreshMode>(m_options.value("refresh_mode").toInt());
}

std::vector<string> CuTangoOptBuilder::fetchProps() const
{
    return m_options.value("fetch_props").toStringVector();
}

CuData CuTangoOptBuilder::options() const
{
    return m_options;
}
