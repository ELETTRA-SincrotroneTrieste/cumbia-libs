#ifndef QUSTRINGLIST_H
#define QUSTRINGLIST_H

#include <QStringList>
#include <vector>
#include <string>
#include <cudatatypes.h>

class CuData;
class CuVariant;

/*! \brief convenience class to build a Qt QStringList from std::vector<std::string> or from a value stored in CuData
 *
 * This class reduces the code that would be normally required to create a QStringList from a C++ std vector of strings.
 * QuStringList *is* a Qt QStringList.
 *
 * QuStringList's can be built from a std::vector<std::string>, a CuData bundle or a CuVariant type, if the data for which
 * conversion is required is known to be either a vector of std::string or another type that can be *converted* to
 * a string representation. Data extraction from CuVariant relies on the CuVariant::toStringVector method.
 */
class QuStringList : public QStringList
{
public:
    QuStringList();
    QuStringList(const std::vector<std::string> &stdvs);
    QuStringList(const CuData &da, const char *key);
    QuStringList(const CuData &da, TTT::Key key);
    QuStringList(const CuVariant &va);
};

#endif // QUSTRINGLIST_H
