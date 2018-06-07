#include "cutangoopt_builder.h"
#include <cuvariant.h>
#include <cudatatypes_ex.h>

CuTangoOptBuilder::CuTangoOptBuilder()
{
    m_options.add(CuXDType::RefreshMode, CuTReader::ChangeEventRefresh);
    m_options.add(CuXDType::FetchHistory, false);
    m_options.add(CuXDType::Period, 1000);
    m_options.add(CuXDType::FetchProperties, std::vector<std::string>());
}

CuTangoOptBuilder::CuTangoOptBuilder(int per, CuTReader::RefreshMode mod)
{
    m_options.add(CuXDType::Period,  per);
    m_options.add(CuXDType::RefreshMode , mod);
    m_options.add(CuXDType::FetchHistory, false);
    m_options.add(CuXDType::FetchProperties, std::vector<std::string>());
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
    m_options.add(CuXDType::FetchProperties, props);
    return *this;
}

CuTangoOptBuilder & CuTangoOptBuilder::addFetchAttProp(const string &s)
{
    std::vector<std::string> prps = m_options.value(CuXDType::FetchProperties).toStringVector();
    prps.push_back(s);
    m_options.add(CuXDType::FetchProperties, prps);
    return *this;
}

CuTangoOptBuilder & CuTangoOptBuilder::setFetchAttHistory(bool fetch)
{
    m_options.add(CuXDType::FetchHistory, fetch);
    return *this;
}

CuTangoOptBuilder & CuTangoOptBuilder::setPeriod(int millis)
{
    m_options.add(CuXDType::Period, millis);
    return *this;
}

CuTangoOptBuilder & CuTangoOptBuilder::setRefreshMode(CuTReader::RefreshMode mode)
{
    m_options.add(CuXDType::RefreshMode, mode);
    return *this;
}

bool CuTangoOptBuilder::operator ==(const CuTangoOptBuilder &other) const
{
    return m_options == other.m_options;
}

int CuTangoOptBuilder::period() const
{
    return m_options.value(CuXDType::Period).toInt();
}

CuTReader::RefreshMode CuTangoOptBuilder::mode() const
{
    return static_cast<CuTReader::RefreshMode>(m_options.value(CuXDType::RefreshMode).toInt());
}

std::vector<string> CuTangoOptBuilder::fetchProps() const
{
    return m_options.value(CuXDType::FetchProperties).toStringVector();
}

CuData CuTangoOptBuilder::options() const
{
    return m_options;
}
