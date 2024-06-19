#include "qustringlist.h"
#include <cudata.h>

QuStringList::QuStringList() : QStringList() {

}

/*! \brief build a QuStringList from a vector of C++ strings
 *
 * @param stdvs a std::vector<std::string> that will be mapped into this QuStringList
 *
 */
QuStringList::QuStringList(const std::vector<std::string> &stdvs)  : QStringList() {
    for(size_t i = 0; i < stdvs.size(); i++)
        append(QString::fromStdString(stdvs[i]));
}

/*! \brief build a QuStringList from the CuData value associated to the given key
 *
 * @param da a const reference to CuData
 * @param key the key used to get the associated value in the given CuData
 *
 * If the given CuData contains the given key, the associated value is converted
 * to QuStringList.
 *
 * \par Note
 * The CuVariant::toStringVector is used prior to QuStringList conversion. If the native type
 * is not std::string, CuVariant::toStringVector tries to convert the elements into their string
 * representation if possible. If conversion fails, the resulting QuStringList will be empty.
 */
QuStringList::QuStringList(const CuData &da, const char *key) : QStringList() {
    if(da.containsKey(key)) {
        bool ok;
        std::vector<std::string> vs = da[key].toStringVector(&ok);
        for(size_t i = 0; i < vs.size() && ok; i++)
            append(QString::fromStdString(vs[i]));
    }
}

QuStringList::QuStringList(const CuData &da, TTT::Key key) : QStringList() {
    if(da.containsKey(key)) {
        bool ok;
        std::vector<std::string> vs = da[key].toStringVector(&ok);
        for(size_t i = 0; i < vs.size() && ok; i++)
            append(QString::fromStdString(vs[i]));
    }
}

/*! \brief build a QuStringList from a CuVariant
 *
 * @param va a CuVariant known to store a vector of std::string elements
 *
 * See comments and notes in the QuStringList(const CuData &da, const char *key) method.
 *
 */
QuStringList::QuStringList(const CuVariant &va) {
    bool ok;
    std::vector<std::string> vs = va.toStringVector(&ok);
    for(size_t i = 0; i < vs.size() && ok; i++)
        append(QString::fromStdString(vs[i]));
}
