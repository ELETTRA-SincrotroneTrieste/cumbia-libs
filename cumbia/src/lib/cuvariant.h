#ifndef CUVARIANT_H
#define CUVARIANT_H

#include <sys/types.h>
#include <vector>
#include <string>

#include <cumacros.h>

#define TIMESTAMPLEN    32
#define SRCLEN          256

class CuVariantPrivate;

/** \brief CuVariant class acts like a union for the *cumbia* data types.
 *
 * A CuVariant object holds a single value of a single getType type at
 * a time. Some types are multi-valued, for example those with
 * DataFormat::Vector format.
 *
 * You can find out what type the CuVariant holds, with CuVariant::getType.
 * The types of data supported are listed in the DataType enumeration.
 * The data formats that is possible to store are declared in the DataFormat
 * enum.
 *
 * For each supported data type and format, appropriate constructors
 * and toXXX() methods are provided to extract the stored value.
 *
 * \par Example
 * \code
 * unsigned int in = 1, out;
 * CuVariant v(in);
 * out = v.toUInt() + 1;
 * \endcode
 *
 * CuVariant is used to store *values* associated to *keys* in CuData
 * bundles. CuData is the object used to exchange data inside the *cumbia*
 * library and between the library and the clients.
 *
 * \par Example: extracting CuVariant values from CuData bundles
 * This example is taken from the QuLabel class in the cumbia-qtcontrols
 * module. You will learn how to extract values from CuData and convert
 * them to the desired type.
 *
 * \code
  // QuLabel inherits from QLabel. Text is changed with *setText*
  void QuLabel::onUpdate(const CuData &da)
  {
    bool read_ok = !da["err"].toBool();
    if(da.containsKey("value")) {
      CuVariant val = da["value"];
      if(val.getType() == CuVariant::Boolean) {
            txt = (val.toBool() ? "OK" : "ERROR" );
        }
        else {
            txt = QString::fromStdString(da["value"].toString());
        }
        setText(txt);
    }
  }
 * \endcode
 */
class CuVariant
{
public:

    /**
     * @brief The DataFormat enum lists the different data formats supported
     *        by CuVariant
     *
     * \li scalar: one single value
     * \li vector: a data array
     * \li matrix: a 2 dimensional matrix of data.
     *
     * @see getFormat
     */
    enum DataFormat { FormatInvalid = 0, ///< invalid format
                      Scalar, ///< a scalar value
                      Vector, ///< a vector, or array
                      Matrix, ///< a two dimensional vector (matrix)
                      EndFormatTypes = 32 ///< upper limit value for format types
                    };

    /**
     * @brief The DataType enum lists the different data types that can be held
     *        by CuVariant
     *
     * @see getType
     */
    enum DataType { TypeInvalid = 0, ///< invalid data type
                    Short, ///< short int
                    UShort, ///< unsigned short int
                    Int, ///< int
                    UInt, ///< unsigned int
                    LongInt, ///< long int
                    LongLongInt, ///< long long int
                    LongUInt, ///< long unsigned int
                    LongLongUInt, ///< unsigned long long
                    Float, ///< float
                    Double, ///< double
                    LongDouble, ///< long double
                    Boolean, ///< bool
                    String, ///< std::string, const char*
                    VoidPtr, ///< generic void*
                    EndDataTypes = 64 ///< defines an upper bound of the enumeration
                  };

    CuVariant(const size_t size, DataFormat df,
             DataType dt);

    /* scalars */
    CuVariant(short int i);

    CuVariant(short unsigned int u);

    CuVariant(int i);

    CuVariant(unsigned int ui);

    CuVariant(long int li);

    CuVariant(long long int li);

    CuVariant(unsigned long int lui);

    CuVariant(unsigned long long int ului);

    CuVariant(float d);

    CuVariant(double d);

    CuVariant(long double ld);

    CuVariant(bool b);

    CuVariant(const std::string &s);

    CuVariant(const char *s);

    CuVariant(void *ptr);


    /* vector */
    CuVariant(const std::vector<short int> &i);

    CuVariant(const std::vector<short unsigned int> &si);

    CuVariant(const std::vector<int> &vi);

    CuVariant(const std::vector<unsigned int> &ui);

    CuVariant(const std::vector<long int> &li);

    CuVariant(const std::vector<long long int> &lli);

    CuVariant(const std::vector<long unsigned int> &lui);

    CuVariant(const std::vector<long long unsigned int> &llui);

    CuVariant(const std::vector<float> &vf);

    CuVariant(const std::vector<double> &vd);

    CuVariant(const std::vector<long double> &vd);

    CuVariant(const std::vector<bool> &vd);

    CuVariant(const std::vector<std::string > &vd);

    CuVariant(const std::vector<void *> &vptr);

    CuVariant(const CuVariant &other);

    CuVariant(CuVariant && other);

    CuVariant();

    CuVariant & operator=(const CuVariant& other);

    CuVariant & operator=(CuVariant&& other);

    bool operator ==(const CuVariant &other) const;

    bool operator !=(const CuVariant &other) const;

    virtual ~CuVariant();

    DataFormat getFormat() const;

    DataType getType() const;

    size_t getSize() const;

    bool isInteger() const;

    bool isFloatingPoint() const;

    bool isVoidPtr() const;

    std::vector<double> toDoubleVector() const;

    std::vector<long double> toLongDoubleVector() const;

    std::vector<float> toFloatVector() const;

    std::vector<int> toIntVector() const;

    std::vector<unsigned int> toUIntVector() const;

    std::vector<unsigned long long int> toLongLongUIntVector() const;

    std::vector<long int> toLongIntVector() const;

    std::vector<long long int> toLongLongIntVector() const;

    std::vector<unsigned long int> toULongIntVector() const;

    std::vector<bool> toBoolVector() const;

    std::vector<short> toShortVector() const;

    std::vector<unsigned short> toUShortVector() const;

    double toDouble(bool *ok = NULL) const;

    long double toLongDouble(bool *ok = NULL) const;

    bool isValid() const;

    bool isNull() const;

    bool isWNull() const;

    unsigned long int toULongInt(bool *ok = NULL) const;

    unsigned long long toULongLongInt(bool *ok) const;

    long int toLongInt( bool *ok = NULL) const;

    long long int toLongLongInt(bool *ok = NULL) const;

    int toInt( bool *ok = NULL) const;

    unsigned int toUInt( bool *ok = NULL) const;

    unsigned short toUShortInt(bool *ok = NULL) const;

    short toShortInt(bool *ok = NULL) const;

    float toFloat(bool *ok = NULL ) const;

    bool toBool(bool *ok = NULL) const;

    std::string toString(bool *ok = NULL, const char *format = "") const;

    std::vector<std::string> toStringVector(bool *ok = NULL) const;

    double *toDoubleP() const;

    long double *toLongDoubleP() const;

    long int *toLongIntP() const;

    long long int* toLongLongIntP() const;

    int *toIntP() const;

    unsigned int *toUIntP() const;

    short int *toShortP() const;

    unsigned short *toUShortP() const;

    unsigned long int *toULongIntP() const;

    float *toFloatP() const;

    bool *toBoolP() const;

    char **toCharP() const;

    void *toVoidP() const;

    template<typename T> bool to(T &val) const;

    template<typename T> bool toVector(std::vector<T> &v) const;

    void append(const CuVariant& other);

    CuVariant &toVector();

    std::string dataFormatStr(int f) const;

    std::string dataTypeStr(int t) const;

private:

    void build_from(const CuVariant& other);

    void cleanup();

    void data_reserve(size_t size);

    template<typename T>void from(T value);

    template<typename T> void from(const std::vector<T> &v);

    void from(const std::vector<std::string > & s);

    void from_std_string(const std::string & s);

    void init(DataFormat df, DataType dt);

    void delete_rdata();

private:

    CuVariantPrivate *d;
};

/* implementation of public template methods must be included
 * by third party users, otherwise we get undefined references
 */
#include <cuvariant_t.h>

#endif // XVARIANT_H
