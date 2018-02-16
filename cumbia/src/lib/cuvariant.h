#ifndef CUVARIANT_H
#define CUVARIANT_H

#include <sys/types.h>
#include <vector>
#include <string>

#include <cumacros.h>

#define TIMESTAMPLEN    32
#define SRCLEN          256

class CuVariantPrivate;

/** \brief Historical database data container.
 *
 * XVariant is aimed at storing data fetched from an historical database.
 * It can contain several kinds of data.
 * The creation and filling of XVariant are carried out by the Hdbextractor
 * library. The end user is normally involved in data extraction for statistics,
 * display or whatever.
 */
class CuVariant
{
public:

    /**
     * @brief The DataFormat enum lists the different data formats supported
     *        by XVariant
     *
     * Scalar: single value
     * Vector: an array (Tango names it spectrum) of data
     * Matrix: a 2 dimensional matrix of data (what Tango calls Image).
     *
     */
    enum DataFormat { FormatInvalid = -1, Scalar, Vector, Matrix, EndFormatTypes };

    /**
     * @brief The DataType enum lists the different data types that can be memorized
     *        into XVariant
     */
    enum DataType { TypeInvalid = -1, Short, UShort, Int, UInt,
                    LongInt, LongUInt, Float, Double,
                    LongDouble, Boolean, String, EndDataTypes };

    CuVariant(const size_t size, DataFormat df,
             DataType dt);

    /* scalars */
    CuVariant(short int i);

    CuVariant(short unsigned int u);

    CuVariant(int i);

    CuVariant(unsigned int ui);

    CuVariant(long int li);

    CuVariant(unsigned long int lui);

    CuVariant(float d);

    CuVariant(double d);

    CuVariant(long double ld);

    CuVariant(bool b);

    CuVariant(const std::string &s);

    CuVariant(const char *s);


    /* vector */
    CuVariant(const std::vector<short int> &i);

    CuVariant(const std::vector<short unsigned int> &si);

    CuVariant(const std::vector<int> &vi);

    CuVariant(const std::vector<unsigned int> &ui);

    CuVariant(const std::vector<long int> &li);

    CuVariant(const std::vector<long unsigned int> &lui);

    CuVariant(const std::vector<float> &vf);

    CuVariant(const std::vector<double> &vd);

    CuVariant(const std::vector<long double> &vd);

    CuVariant(const std::vector<bool> &vd);

    CuVariant(const std::vector<std::string > &vd);

    CuVariant(const CuVariant &other);

    CuVariant(CuVariant && other);

    CuVariant();

    CuVariant & operator=(const CuVariant& other);

    CuVariant & operator=(CuVariant&& other);

    bool operator ==(const CuVariant &other) const;

    bool operator !=(const CuVariant &other) const;

    virtual ~CuVariant();

    const char *getSource() const;

    DataFormat getFormat() const;

    DataType getType() const;

    size_t getSize() const;

    bool isInteger() const;

    bool isFloatingPoint() const;

    std::vector<double> toDoubleVector() const;

    std::vector<long double> toLongDoubleVector() const;

    std::vector<float> toFloatVector() const;

    std::vector<int> toIntVector() const;

    std::vector<unsigned int> toUIntVector() const;

    std::vector<long int> toLongIntVector() const;

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

    long int toLongInt( bool *ok = NULL) const;

    int toInt( bool *ok = NULL) const;

    unsigned int toUInt( bool *ok = NULL) const;

    unsigned short toUShortInt(bool *ok = NULL) const;

    short toShortInt(bool *ok = NULL) const;

    float toFloat(bool *ok = NULL ) const;

    bool toBool(bool *ok = NULL) const;

    const char *getError() const;

    void setError(const char *error);

    std::string toString(bool *ok = NULL, const char *double_format = "%f") const;

    std::vector<std::string> toStringVector(bool *ok = NULL) const;

    double *toDoubleP() const;

    long double *toLongDoubleP() const;

    long int *toLongIntP() const;

    int *toIntP() const;

    unsigned int *toUIntP() const;

    short int *toShortP() const;

    unsigned short *toUShortP() const;

    unsigned long int *toULongIntP() const;

    float *toFloatP() const;

    bool *toBoolP() const;

    char **toCharP() const;

    template<typename T> bool to(T &val) const;

    template<typename T> bool toVector(std::vector<T> &v) const;

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
