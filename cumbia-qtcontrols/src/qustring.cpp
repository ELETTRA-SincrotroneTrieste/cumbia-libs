#include "qustring.h"
#include <cudata.h>

QuString::QuString() : QString(){
}

/*! \brief builds a QuString from a std::string
 *
 * @param stds a C++ string
 */
QuString::QuString(const std::string &stds) : QString(QString::fromStdString(stds)) {
}

/*! \brief build a QuString from the CuData value associated to the given key
 *
 * @param da a const reference to CuData
 * @param key the key used to get the associated value in the given CuData
 *
 * If the given CuData contains the given key, the associated value is converted
 * to QuString.
 *
 * \par Note
 * The CuVariant::toString is used before QuString conversion. If the native type
 * is not std::string, CuVariant::toString tries to convert the value into its string
 * representation. If conversion fails, the resulting QuString will be empty.
 *
 */
QuString::QuString(const CuData &da, const char *key) : QString() {
    if(da.containsKey(key)) {
        bool ok;
        std::string s = da[key].toString(&ok);
        if(ok) append(QString::fromStdString(s));
    }
}

QuString::QuString(const CuData &da, const CuDType::Key key) : QString() {
    if(da.containsKey(key)) {
        bool ok;
        std::string s = da[key].toString(&ok);
        if(ok) append(QString::fromStdString(s));
    }
}

/*! \brief build a QuString from a CuVariant
 *
 * @param va a CuVariant known to store a std::string or any type that can be represented by a string
 *
 * See comments and notes in the QuString(const CuData &da, const char *key) method.
 *
 */
QuString::QuString(const CuVariant &va) : QString() {
    bool ok;
    std::string s = va.toString(&ok);
    if(ok)  append(QString::fromStdString(s));
}
