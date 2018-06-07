#ifndef CUTANGOREADOPTIONS_H
#define CUTANGOREADOPTIONS_H

#include <cudatatypes_ex.h>
#include <cutreader.h>
#include <vector>
#include <string>
#include <cudata.h>

/** \brief set and get options to configure readers and writers for Tango
 *
 * This is a wrapper to CuData exposing Tango specific methods to set and get configuration
 * options.
 */
class CuTangoOptBuilder
{
public:

    CuTangoOptBuilder();

    CuTangoOptBuilder(int per, CuTReader::RefreshMode mod);

    CuTangoOptBuilder(const CuData& d);

    CuTangoOptBuilder(const CuTangoOptBuilder &other);

    CuTangoOptBuilder& setFetchAttProps(const std::vector<std::string> & props);

    CuTangoOptBuilder & addFetchAttProp(const std::string& s);

    CuTangoOptBuilder & setFetchAttHistory(bool fetch = true);

    CuTangoOptBuilder & setPeriod(int millis);

    CuTangoOptBuilder & setRefreshMode(CuTReader::RefreshMode mode);

    bool operator ==(const CuTangoOptBuilder& other) const;

    int period() const;

    CuTReader::RefreshMode mode() const;

    std::vector<std::string> fetchProps() const;

    bool fetchHistory() const;

    CuData options() const;

private:
    CuData m_options;
};

#endif // CUTREADOPTIONS_H
