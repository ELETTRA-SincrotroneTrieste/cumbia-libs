#ifndef QUSTRING_H
#define QUSTRING_H

#include <QString>
#include <cudata.h>

class CuVariant;

/*! \brief convenience class to build a Qt QString from an std::string or from a value stored in CuData
 *
 * This class reduces the code that would be normally required to create a QString from a C++ std string.
 * QuString *is* a Qt QString.
 *
 * QuString can be built from a std::vector<std::string>, a CuData bundle or a CuVariant type, if the data for which
 * conversion is required is known to be either a std::string or another type that can be *converted* to
 * a string representation. Data extraction from CuVariant relies on the CuVariant::toString method.
 */
class QuString : public QString
{
public:
    QuString();

    QuString(const std::string &stds);
    QuString(const CuData &da, const char *key);
    QuString(const CuData &da, const TTT::Key key);
    QuString(const CuVariant &va);
};

#endif // QUSTRING_H
