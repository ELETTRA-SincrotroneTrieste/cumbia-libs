#ifndef CUVARIANT_H
#define CUVARIANT_H

#include <sys/types.h>
#include <vector>
#include <string>

#include <cumacros.h>
#include <cumatrix.h>

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
 * \par Implicit sharing
 * \since 1.4.0
 *
 * Data is implicitly shared, enabling *copy on write*. Atomic reference counters make the class
 * reentrant, meaning that methods can be called from multiple threads, but only if each invocation
 * uses its own data. Atomic reference counting is used to ensure the integrity of the shared data.
 * Since cumbia 1.4.0, CuData uses implicit sharing as well.
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
		            Char,
		            UChar,
                    EndDataTypes = 64 ///< defines an upper bound of the enumeration
                  };

    CuVariant(const size_t size, DataFormat df,
             DataType dt);

    virtual ~CuVariant();

    /* scalars */
    CuVariant(char c);
    CuVariant(unsigned char uc);
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

    /* matrix, since 1.2.5 */
    CuVariant(const std::vector<unsigned char> &m, size_t dimx, size_t dimy);
    CuVariant(const std::vector<char> &m, size_t dimx, size_t dimy);
    CuVariant(const std::vector<short int> &i, size_t dimx, size_t dimy);
    CuVariant(const std::vector<short unsigned int> &si, size_t dimx, size_t dimy);
    CuVariant(const std::vector<int> &vi, size_t dimx, size_t dimy);
    CuVariant(const std::vector<unsigned int> &ui, size_t dimx, size_t dimy);
    CuVariant(const std::vector<long int> &li, size_t dimx, size_t dimy);
    CuVariant(const std::vector<long long int> &lli, size_t dimx, size_t dimy);
    CuVariant(const std::vector<long unsigned int> &lui, size_t dimx, size_t dimy);
    CuVariant(const std::vector<long long unsigned int> &llui, size_t dimx, size_t dimy);
    CuVariant(const std::vector<float> &vf, size_t dimx, size_t dimy);
    CuVariant(const std::vector<double> &vd, size_t dimx, size_t dimy);
    CuVariant(const std::vector<long double> &vd, size_t dimx, size_t dimy);
    CuVariant(const std::vector<bool> &vb, size_t dimx, size_t dimy);
    CuVariant(const std::vector<std::string > &vs, size_t dimx, size_t dimy);

    /* vector */
    CuVariant(const std::vector<char> &c);
    CuVariant(const std::vector<unsigned char> &uc);
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

    /* pointer */
    template <typename T> CuVariant(const T* p, size_t siz, DataFormat f, DataType t);

    CuVariant & operator=(const CuVariant& other);
    CuVariant & operator=(CuVariant&& other);
    bool operator ==(const CuVariant &other) const;
    bool operator !=(const CuVariant &other) const;

    DataFormat getFormat() const;
    DataType getType() const;
    size_t getSize() const;

    bool isInteger() const;
    bool isUnsignedType() const;
    bool isSignedType() const;
    bool isFloatingPoint() const;
    bool isVoidPtr() const;

	bool isValid() const;
	bool isNull() const;
	bool isWNull() const;

    std::vector<double> toDoubleVector() const;
    std::vector<double> dv() const { return toDoubleVector(); }
    std::vector<long double> toLongDoubleVector() const;
    std::vector<long double> ldv() const { return toLongDoubleVector(); }
    std::vector<float> toFloatVector() const;
    std::vector<float> fv() const { return toFloatVector(); }
    std::vector<int> toIntVector() const;
    std::vector<int> iv() const { return toIntVector(); }
    std::vector<unsigned int> toUIntVector() const;
    std::vector<unsigned int> uv() const { return toUIntVector(); }
    std::vector<unsigned long long int> toLongLongUIntVector() const;
    std::vector<unsigned long long int> ullv() const { return toLongLongUIntVector(); }
    std::vector<long int> toLongIntVector() const;
    std::vector<long int> lv() const { return toLongIntVector(); }
    std::vector<long long int> toLongLongIntVector() const;
    std::vector<long long int> llv() { return toLongLongIntVector(); }
    std::vector<unsigned long int> toULongIntVector() const;
    std::vector<unsigned long int> ulv() const { return toULongIntVector(); }

    std::vector<bool> toBoolVector() const;
    std::vector<bool> bv() const { return toBoolVector(); }
    std::vector<short> toShortVector() const;
    std::vector<short> siv() const { return toShortVector(); }

    std::vector<unsigned short> toUShortVector() const;
    std::vector<unsigned short> usv() const { return toUShortVector(); }
    std::vector<char> toCharVector() const;
    std::vector<char> cv() const { return toCharVector(); }
    std::vector<unsigned char> toUCharVector() const;
    std::vector<unsigned char> ucv() const { return toUCharVector(); }

    double toDouble(bool *ok = nullptr) const;
    double d() const;
    long double toLongDouble(bool *ok = nullptr) const;
    long double ld() const;
    unsigned long int toULongInt(bool *ok = nullptr) const;
    unsigned long int ul() const;
    unsigned long long toULongLongInt(bool *ok) const;
    unsigned long long ull() const;
    long int toLongInt( bool *ok = nullptr) const;
    long int l() const;
    long long int toLongLongInt(bool *ok = nullptr) const;
    long long int ll() const;
    int toInt( bool *ok = nullptr) const;
    int i() const;
    unsigned int toUInt( bool *ok = nullptr) const;
    unsigned int u() const;
    unsigned short toUShortInt(bool *ok = nullptr) const;
    unsigned short us() const;
    short toShortInt(bool *ok = nullptr) const;
    short si() const;
    float toFloat(bool *ok = nullptr ) const;
    float f() const;
    bool toBool(bool *ok = nullptr) const;
    bool b() const;
    std::string toString(bool *ok = nullptr, const char *format = "") const;
    std::string s() const;
    std::string s(const char *fmt, bool *ok = nullptr) const;

    std::vector<std::string> toStringVector(bool *ok = nullptr) const;
    std::vector<std::string> toStringVector( const char *fmt) const;
    std::vector<std::string> toStringVector( const char *fmt, bool *ok) const;
    std::vector<std::string> sv() const;
    std::vector<std::string> sv(const char *fmt, bool *ok = nullptr) const;

    double *toDoubleP() const;
    long double *toLongDoubleP() const;
    long int *toLongIntP() const;
    long long int* toLongLongIntP() const;
    int *toIntP() const;
    unsigned int *toUIntP() const;
    short int *toShortP() const;
    unsigned short *toUShortP() const;
    unsigned long int *toULongIntP() const;
    unsigned long long *toULongLongIntP() const;
    float *toFloatP() const;
	bool *toBoolP() const;
    unsigned char* toUCharP() const;
    char* toCharP() const;

	char **to_C_charP() const;
    void *toVoidP() const;

    template<typename T> CuMatrix<T> toMatrix() const;

    template<typename T> bool to(T &val) const;
    template<typename T> bool toVector(std::vector<T> &v) const;

    void append(const CuVariant& other);
    CuVariant &toVector();

    std::string dataFormatStr(int f) const;
    std::string dataTypeStr(int t) const;

private:

    void build_from(const CuVariant& other);
    void m_cleanup();
    template<typename T>void m_from(T value);
    template<typename T> void m_from(const std::vector<T> &v);
    template<typename T> void m_v_to_matrix(const std::vector<T> &v, size_t dimx, size_t dim_y);
    void m_v_to_string_matrix(const std::vector<std::string> &vs, size_t dimx, size_t dim_y);
    void m_from(const std::vector<std::string > & s);
    void m_from_std_string(const std::string & s);
    void m_init(DataFormat df, DataType dt);
    void m_delete_rdata();
    void m_detach();

    CuVariantPrivate *_d;
};

/* implementation of public template methods must be included
 * by third party users, otherwise we get undefined references
 */
#include <cuvariant_t.h>

#endif // XVARIANT_H
