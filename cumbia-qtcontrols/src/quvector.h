#ifndef QUVECTOR_H
#define QUVECTOR_H

#include <QVector>
#include <vector>
#include <cudata.h>

class CuVariant;

/*! \brief convenience class that can be used to quickly convert a std::vector of a given type
 *         into a QVector of the *same type*
 *
 * QuVector *is a QVector* and can be used to convert immediately *std::vector* based data into Qt
 * QVector. Tedious and repetitive code writing is made unnecessary thanks to the versatile class
 * constructors.
 *
 * \par Example usage 1
 * Using QuVector you will be able to write
 *
 * \code
    void Writer::hDataReady(const CuData& v)
    {
        QVector<double> readData = QuVector<double>(v[TTT::Value].toDoubleVector());  // v["value"]
        // ...
    }
 * \endcode
 *
 * instead of
 * \code
    void Writer::vDataReady(const CuData &v)
    {
        QVector<double> readData = QVector<double>::fromStdVector( v[TTT::Value].toDoubleVector())  // v["value"]
        // ...
    }
 * \endcode
 *
 * \par Example usage 2
 * This example builds a QVector directly from CuData
 *
 * \par Note
 * Data contains the *value* key and the data is a vector.
 *
 * \code
    void Writer::vDataReady(const CuData &v)
    {
        // if v.containsKey("value") and v["value"] is a vector
        // qv will contain v["value"] converted to QVector
        QVector<double> qv = QuVector<double>(v);
 * \endcode
 *
 * As you can see in the method documentation, any key can be specified to have its value
 * converted to QuVector
 */
template <typename T>
class QuVector : public QVector<T>
{
public:
    QuVector(const std::vector<T> &);
    QuVector(const CuData& d, const char* key = "value");
    QuVector(const CuVariant& v);
    QuVector();
};

/*! \brief build a QuVector from a std::vector<T>
 *
 * The QuVector can be assigned to a QVector of the same type
 *
 * \par Example
 * \code
 * std::vector<double> stdv; // ... stdv.push_back(...)
 * QVector<double> qv = QuVector<double>(stdv);
 * \endcode
 */
template <typename T>
QuVector<T>::QuVector(const std::vector<T> &v) : QVector<T>() {
    for(size_t i = 0; i < v.size(); i++)
        this->append(v[i]);
}

/*! \brief convenience constructor that builds a QuVector starting from the *vector type* value
 *         associated to the given *key* that must be found in *d*
 *
 * @param d CuData that must contain the *key* with a value of type *std::vector*
 * @param key the name of the key which value will be accessed. Default: "*value*"
 * @return a QuVector with the same elements as those in the std::vector value
 *
 * \par Example. Convert into QVector<double> the *w_value* value, known to be a vector data type
 *
    void onUpdate(const CuData &da) {
       QVector<double> setpoint_v = QuVector<double>(da, "w_value");
       // foreach(double setp, setpoint_v)
       //   ...
    }
 */
template <typename T>
QuVector<T>::QuVector(const CuData &d, const char* key) : QVector<T>()
{
    if(d.containsKey(key)) {
        std::vector<T> stdv;
        bool ok = d[key].toVector<T>(stdv);
        for(size_t i = 0; i < stdv.size() && ok; i++)
            this->append(stdv[i]);
    }
}

/*! \brief convenience constructor that attempts to build a QuVector from a CuVariant, that must be
 *         a std::vector type
 *
 * @param v a CuVariant known to be a std::vector of a given type T
 * @see QuVector(const CuData &d, const char* key)
 */
template<typename T>
QuVector<T>::QuVector(const CuVariant &v) : QVector<T>()
{
    std::vector<T> stdv;
    bool ok = v.toVector<T>(stdv);
    for(size_t i = 0; i < stdv.size() && ok; i++)
        this->append(stdv[i]);
}

/*! \brief empty constructor
 *
 * Builds an empty QuVector
 */
template<typename T>
QuVector<T>::QuVector() : QVector<T>() {

}


#endif // QUVECTOR_H
