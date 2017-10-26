#include "cuvariant.h"
#include <errno.h>
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <time.h>
#include <math.h> /* round */
#include <string.h> /* strerror */
#include "cuvariantprivate.h"
#include "cumacros.h"

void CuVariant::delete_rdata()
{
    if(d->val != NULL)
    {
        /*
         * num DataType { TypeInvalid = -1, Short, UShort, Int, UInt,
         *           LongInt, LongUInt, Float, Double,
         *           LongDouble, Boolean, String, EndDataTypes };
         */
        if(d->type == Double)
            delete [] (double *) d->val;
        else if(d->type == UInt)
            delete [] (unsigned int *) d->val;
        else if(d->type == Int)
            delete [] (int *) d->val;
        else if(d->type == Boolean)
            delete [] static_cast<bool *>(d->val);
        else if(d->type == String)
            delete [] (char *) d->val;
        else if(d->type == Float)
            delete [] (float *) d->val;
        else if(d->type == LongDouble)
            delete [] (long double *) d->val;
        else if(d->type == LongInt)
            delete [] (long int *) d->val;
        else if(d->type == LongUInt)
            delete []  (long unsigned int *) d->val;
        else if(d->type == Short)
            delete [] (short * ) d->val;
        else if(d->type == UShort)
            delete [] (unsigned short *) d->val;
        //        cuprintf("delete_rdata: XVariant %p deleted d %p d->val %p type %d\n", this, d, d->val, d->dataType);
        d->val = NULL;
    }
}

void CuVariant::cleanup()
{
    if(d != NULL)
    {
        if(d->mSize > 0 && d->type == String && d->val != NULL)
        {
            char **ssi = (char **) d->val;
            for(size_t i = 0; i < d->mSize; i++)
            {
                char *si = (char *) ssi[i];
                delete[] si;
            }
        }
        delete_rdata();
        delete d;
        d = NULL;
    }
}

CuVariant::~CuVariant()
{
    // pdelete("~CuVariant: cleaning up %p", this);
    cleanup();
}


CuVariant::CuVariant(short i)
{
    d = new CuVariantPrivate(); /* allocates XVariantDataInfo */
    init(Scalar, Short);
    from(i);
}

CuVariant::CuVariant(unsigned short u)
{
    d = new CuVariantPrivate(); /* allocates XVariantDataInfo */
    init(Scalar, UShort);
    from(u);
}

CuVariant::CuVariant(int i)
{
    d = new CuVariantPrivate(); /* allocates XVariantDataInfo */
    init(Scalar, Int);
    from(i);
}

CuVariant::CuVariant(unsigned int ui)
{
    d = new CuVariantPrivate(); /* allocates XVariantDataInfo */
    init(Scalar, UInt);
    from(ui);
}

CuVariant::CuVariant(long int li)
{
    d = new CuVariantPrivate(); /* allocates XVariantDataInfo */
    init(Scalar, LongInt);
    from(li);
}

CuVariant::CuVariant(long unsigned int lui)
{
    d = new CuVariantPrivate(); /* allocates XVariantDataInfo */
    init(Scalar, LongUInt);
    from(lui);
}

CuVariant::CuVariant(float f)
{
    d = new CuVariantPrivate(); /* allocates XVariantDataInfo */
    init(Scalar, Float);
    from(f);
}

CuVariant::CuVariant(double dou)
{
    d = new CuVariantPrivate(); /* allocates XVariantDataInfo */
    init(Scalar, Double);
    from(dou);
}

CuVariant::CuVariant(long double ld)
{
    d = new CuVariantPrivate(); /* allocates XVariantDataInfo */
    init(Scalar, LongDouble);
    from(ld);
}

CuVariant::CuVariant(bool b)
{
    d = new CuVariantPrivate(); /* allocates XVariantDataInfo */
    init(Scalar, Boolean);
    from(b);
}

CuVariant::CuVariant(const std::string&  s)
{
    d = new CuVariantPrivate(); /* allocates XVariantDataInfo */
    init(Scalar, String);
    from_std_string(s);
}

CuVariant::CuVariant(const char *s)
{
    d = new CuVariantPrivate(); /* allocates XVariantDataInfo */
    init(Scalar, String);
    from_std_string(std::string(s));
}

CuVariant::CuVariant(const std::vector<double> &vd)
{
    d = new CuVariantPrivate(); /* allocates XVariantDataInfo */
    init(Vector, Double);
    from(vd);
}

CuVariant::CuVariant(const std::vector<long double> &vd)
{
    d = new CuVariantPrivate(); /* allocates XVariantDataInfo */
    init(Vector, LongDouble);
    from(vd);
}

CuVariant::CuVariant(const std::vector<bool> &vb)
{
    d = new CuVariantPrivate(); /* allocates XVariantDataInfo */
    init(Vector, Boolean);
    from(vb);
}

CuVariant::CuVariant(const std::vector<std::string> &vs)
{
    d = new CuVariantPrivate(); /* allocates XVariantDataInfo */
    init(Vector, String);
    from(vs);
}

CuVariant::CuVariant(const std::vector<short> &si)
{
    d = new CuVariantPrivate(); /* allocates XVariantDataInfo */
    init(Vector, Short);
    from(si);
}

CuVariant::CuVariant(const std::vector<unsigned short> &si)
{
    d = new CuVariantPrivate(); /* allocates XVariantDataInfo */
    init(Vector, UShort);
    from(si);
}

CuVariant::CuVariant(const std::vector<int> &vi)
{
    d = new CuVariantPrivate(); /* allocates XVariantDataInfo */
    init(Vector, Int);
    from(vi);
}

CuVariant::CuVariant(const std::vector<unsigned int> &vi)
{
    d = new CuVariantPrivate(); /* allocates XVariantDataInfo */
    init(Vector, UInt);
    from(vi);
}

CuVariant::CuVariant(const std::vector<long> &li)
{
    d = new CuVariantPrivate(); /* allocates XVariantDataInfo */
    init(Vector, LongInt);
    from(li);
}

CuVariant::CuVariant(const std::vector<unsigned long> &lui)
{
    d = new CuVariantPrivate(); /* allocates XVariantDataInfo */
    init(Vector, LongUInt);
    from(lui);
}

CuVariant::CuVariant(const std::vector<float> &vf)
{
    d = new CuVariantPrivate(); /* allocates XVariantDataInfo */
    init(Vector, Float);
    from(vf);
}

/** \brief Creates an empty (invalid) XVariant
 *
 * The XVariant created is invalid.
 */
CuVariant::CuVariant()
{
    d = new CuVariantPrivate();  /* allocates XVariantDataInfo */
    d->format = FormatInvalid;
    d->type = TypeInvalid;
}

/** \brief copy constructor
 *
 * Create a new variant initialized from the values of the other parameter
 *
 * @param other the XVariant to be cloned.
 *
 */
CuVariant::CuVariant(const CuVariant &other)
{
    build_from(other);
}

CuVariant::CuVariant(CuVariant &&other)
{
    /* no new d here! */
    d = other.d;
    other.d = NULL; /* don't delete */
}

CuVariant & CuVariant::operator=(const CuVariant& other)
{
    if(this != &other)
    {
        cleanup();
        build_from(other);
    }
    return *this;
}

CuVariant &CuVariant::operator=(CuVariant &&other)
{
    if(this != &other)
    {
        cleanup();
        d = other.d;
        other.d = NULL; /* don't delete */
    }
    return *this;
}

bool CuVariant::operator ==(const CuVariant &other) const
{
    cuprintf(">>> CuVariant::operator ==\e[1;33mWARNING WARNING WARNING --- CHECK ME --- \e[0m\n");
    if(! (other.getFormat() ==  this->getFormat() && other.getSize() ==  this->getSize() &&
          other.getType() ==  this->getType() && other.isNull() ==  this->isNull() &&
          other.isValid() ==  this->isValid() ) )
        return false;
    /* one d->val is null and the other not */
    if( (other.d->val == NULL &&  this->d->val != NULL) || (other.d->val != NULL &&  this->d->val == NULL) )
        return false;

    char **v_str = NULL, **other_v_str = NULL;
    /*
    * enum DataType { TypeInvalid = -1, Short, UShort, Int, UInt,
                    LongInt, LongUInt, Double,
                    LongDouble, Boolean, String, EndDataTypes };
    */
    switch (d->type) {
    case Short:
        return memcmp(other.d->val,  this->d->val, sizeof(short) * d->mSize) == 0;
    case UShort:
        return memcmp(other.d->val,  this->d->val, sizeof(unsigned short) * d->mSize) == 0;
    case Int:
        return memcmp(other.d->val,  this->d->val, sizeof(int) * d->mSize) == 0;
    case UInt:
        return memcmp(other.d->val,  this->d->val, sizeof(unsigned int) * d->mSize) == 0;
    case LongInt:
        return memcmp(other.d->val,  this->d->val, sizeof(long int) * d->mSize) == 0;
    case LongUInt:
        return memcmp(other.d->val,  this->d->val, sizeof(long unsigned int) * d->mSize) == 0;
    case Float:
        return memcmp(other.d->val,  this->d->val, sizeof(float) * d->mSize) == 0;
    case Double:
        return memcmp(other.d->val,  this->d->val, sizeof(double) * d->mSize) == 0;
    case LongDouble:
        return memcmp(other.d->val,  this->d->val, sizeof(long double) * d->mSize) == 0;
    case Boolean:
        return memcmp(other.d->val,  this->d->val, sizeof(bool) * d->mSize) == 0;
    case String:
        v_str = static_cast<char **>(d->val);
        other_v_str = static_cast<char **>(other.d->val);
        if(d->mSize == 1)
        {
            return strcmp(v_str[0], other_v_str[0]) == 0;
        }
        else
        {
            cuprintf(">>> CuVariant::operator ==\e[1;31mWARNING WARNING WARNING!!! STRING VECTOR COMPARISON TO BE IMPLEMENTED IN OPERATOR ==\e[0m\n");
            cuprintf(">>> CuVariant::operator ==\e[1;31mWARNING WARNING WARNING   RETURNING FALSE\e[0m\n");
        }
        return false;
    default:
        break;
    }
    return false;
}

bool CuVariant::operator !=(const CuVariant &other) const
{
    return !this->operator ==(other);
}

void CuVariant::build_from(const CuVariant& other)
{
    d = new CuVariantPrivate();

    d->format  = other.getFormat();
    d->type = other.getType();
    d->mSize = other.getSize();
    d->mIsValid = other.isValid();
    d->mIsNull = other.isNull();

    //cuprintf("\e[0;36mCuVariant::build_from: %p copy from %p this->d: %p: format %d size %ld\e[0m \n", this, &other, d,
    //      d->format,  d->mSize);

    if(d->type == CuVariant::Double)
    {
        double *vd = new double[d->mSize];
        for(size_t i = 0; i < d->mSize; i++)
            vd[i] =  other.toDoubleP()[i];
        d->val = vd;
    }
    else if(d->type == CuVariant::Int)
    {
        int *vi =  new int[d->mSize];

        for(size_t i = 0; i < d->mSize; i++)
            vi[i] = other.toIntP()[i];
        d->val = vi;
    }
    else if(d->type == CuVariant::UInt)
    {
        unsigned int *vi =  new unsigned int[d->mSize];

        for(size_t i = 0; i < d->mSize; i++)
            vi[i] = other.toUIntP()[i];
        d->val = vi;
    }
    else if(d->type == CuVariant::LongInt)
    {
        long int *vi =  new long int[d->mSize];

        for(size_t i = 0; i < d->mSize; i++)
            vi[i] = other.toLongIntP()[i];
        d->val = vi;
    }
    else if(d->type == CuVariant::Boolean)
    {
        bool *vb = new bool[d->mSize];
        for(size_t i = 0; i < d->mSize; i++)
            vb[i] = other.toBoolP()[i];
        d->val = vb;
    }
    else if(d->type == CuVariant::Short)
    {
        short *vs = new short[d->mSize];
        for(size_t i = 0; i < d->mSize; i++)
            vs[i] = other.toShortP()[i];
        d->val = vs;
    }
    else if(d->type == CuVariant::UShort)
    {
        unsigned short *vus = new unsigned short[d->mSize];
        for(size_t i = 0; i < d->mSize; i++)
            vus[i] = other.toUShortP()[i];
        d->val = vus;
    }
    else if(d->type == CuVariant::Float)
    {
        float *vf = new float[d->mSize];
        for(size_t i = 0; i < d->mSize; i++)
            vf[i] = other.toFloatP()[i];
        d->val = vf;
    }
    else if(d->type == CuVariant::String)
    {
        char *other_s;
        char **str = new char *[d->mSize];
        size_t len;
        for(size_t i = 0; i < d->mSize; i++)
        {
            other_s = other.toCharP()[i];
            if(other_s != NULL)
            {
                len = strlen(other_s);
                str[i] = new char[len + 1];
                memset(str[i], 0, sizeof(char) * (len + 1));
                strncpy(str[i], other_s, len);
            }
            else
                str[i] = NULL;
        }
        d->val = str;
    }
}

/** \brief Cuery the format of the data stored in the XVariant
 *
 * @return the DataFormat (XVariant::Vector, XVariant::Scalar, XVariant::Matrix)
 */
CuVariant::DataFormat CuVariant::getFormat() const
{
    return static_cast<CuVariant::DataFormat>(d->format);
}

/** \brief Returns the DataType stored by XVariant
 *
 */
CuVariant::DataType CuVariant::getType() const
{
    return static_cast<CuVariant::DataType>(d->type);
}

/** \brief Returns whether the data stored by XVariant is valid
 *
 * @return true the data contained by XVariant is valid
 * @return false the data contained by XVariant is not valid (see getError)
 *
 * @see getError
 * @see isNull
 *
 * \note
 * isValid returns true if data is NULL. In other words, NULL values in the database
 * are deemed valid.
 *
 */
bool CuVariant::isValid() const
{
    return d->mIsValid;
}

/** \brief Returns whether the the data stored by XVariant is NULL or not
 *
 * @return true the data contained by XVariant is NULL
 * @return false the data contained by XVariant is not NULL (see isValid)
 *
 * \note
 * If isNull is true, then isValid will be true also, because a NULL XVariant represents
 * a NULL value stored into the database, which is perfectly legit.
 *
 * @see getError
 * @see isValid
 * @see isWNull
 * @see setCuality
 *
 */
bool CuVariant::isNull() const
{
    return d->mIsNull;
}

/** \brief Returns the size of the data stored by the XVariant
 *
 * @return the size of the data stored by the XVariant. This method is useful to
 *         know the size of a vector of data, in case XVariant encloses spectrum
 *         Tango attributes.
 */
size_t CuVariant::getSize() const
{
    return d->mSize;
}

void CuVariant::init(DataFormat df, DataType dt)
{
    d->mIsValid = (dt > TypeInvalid && dt < EndDataTypes) && (df > FormatInvalid && df < EndFormatTypes);
    d->mSize = 0;
    d->format = df;
    d->type = dt;
    d->mIsNull = true;
}

template<typename T>
void CuVariant::from(const std::vector<T> &v)
{
    if(!d->mIsValid || d->type == String)
        perr("CuVariant::from <vector T>: invalid data type or format. Have you called init first??");
    else
    {
        d->val = NULL;
        d->mSize = v.size();

        d->val = (T *) new T[d->mSize];
        for(size_t i = 0; i < d->mSize; i++)
            static_cast<T *>(d->val)[i] = (T) v[i];

        d->mIsNull = false;
    }
}

void CuVariant::from(const std::vector<std::string> &s)
{
    d->val = NULL;
    if(!d->mIsValid || d->type != String)
        perr("CuVariant::from <vector std::string>: invalid data type or format. Have you called init first??");
    else /* strings need alloc and strcpy */
    {
        d->mSize = s.size();
        char **str_array = new char*[d->mSize];
        for(size_t i = 0; i < d->mSize; i++)
        {
            str_array[i] = new char[s[i].size() + 1];
            strncpy(str_array[i], s[i].c_str(), s[i].size() + 1);
        }
        d->mIsNull = false;
        d->val = str_array;
    }
}

void CuVariant::from_std_string(const std::string &s)
{
    d->mSize = 1;
    size_t size = strlen(s.c_str()) + 1;
    char **str = new char*[d->mSize];
    str[0] = new char[size];
    strncpy(str[0], s.c_str(), sizeof(char) * size);
    d->mIsNull = false;
    d->val = str;
}

template<typename T>
void CuVariant::from(T value)
{
    if(!d->mIsValid || d->type == String)
        perr("CuVariant::from <T>: invalid data type or format. Have you called init first??");
    else
    {
        d->mSize = 1;
        d->val = (T *) new T[d->mSize];
        *(static_cast<T *> (d->val) ) = (T) value;
        d->mIsNull = false;
    }

}

/** \brief The conversion method that tries to convert the stored data into a vector of double
 *
 * @return a std vector of double representing the data saved into XVariant
 *
 */
std::vector<double> CuVariant::toDoubleVector() const
{
    double *d_val;
    d_val = (double *) d->val;
    std::vector<double> dvalues(d_val, d_val + d->mSize);
    return dvalues;
}

std::vector<long double> CuVariant::toLongDoubleVector() const
{
    long double *d_val;
    d_val = (long double *) d->val;
    std::vector<long double> ldvalues(d_val, d_val + d->mSize);
    return ldvalues;
}

std::vector<float> CuVariant::toFloatVector() const
{
    float *fval;
    fval =  static_cast<float*>(d->val);
    std::vector<float> fvalues(fval, fval + d->mSize);
    return fvalues;
}

std::vector<int> CuVariant::toIntVector() const
{
    int *i_val = (int *) d->val;
    std::vector<int> ivalues(i_val, i_val + d->mSize);
    return ivalues;
}

std::vector<unsigned int> CuVariant::toUIntVector() const
{
    unsigned int *i_val = (unsigned int *) d->val;
    std::vector<unsigned int> uivalues(i_val, i_val + d->mSize);
    return uivalues;
}

/** \brief The conversion method that tries to convert the stored data into a vector of
 *         unsigned long integers
 *
 * @return a std vector of int representing the data saved into XVariant
 *
 * \note unsigned shorts and unsigned ints are mapped to unsigned long.
 *
 */
std::vector<unsigned long int> CuVariant::toULongIntVector() const
{
    unsigned long int *i_val = (unsigned long int *) d->val;
    std::vector<unsigned long int> ivalues(i_val, i_val + d->mSize);
    return ivalues;
}

/** \brief The conversion method that tries to convert the stored data into a vector of
 *         unsigned long integers
 *
 * @return a std vector of int representing the data saved into XVariant
 * \note unsigned shorts and unsigned ints are mapped to unsigned long
 *
 */
std::vector<long int> CuVariant::toLongIntVector() const
{
    long int *i_val = (long int *) d->val;
    std::vector<long int> ivalues(i_val, i_val + d->mSize);
    return ivalues;
}

/** \brief The conversion method that tries to convert the stored data into a vector of booleans
 *
 * @return a std vector of bool representing the data saved into XVariant
 *
 */
std::vector<bool> CuVariant::toBoolVector() const
{
    bool *b_val = (bool *) d->val;
    std::vector<bool> bvalues(b_val, b_val + d->mSize);
    return bvalues;
}

std::vector<short> CuVariant::toShortVector() const
{
    short *v_s = static_cast<short *>(d->val);
    std::vector<short> svals(v_s, v_s + d->mSize);
    return svals;
}

std::vector<unsigned short> CuVariant::toUShortVector() const
{
    unsigned short *v_us = static_cast<unsigned short *>(d->val);
    std::vector<unsigned short> usvals(v_us, v_us + d->mSize);
    return usvals;
}

/** \brief The conversion method that tries to convert the stored data into a double scalar
 *
 * @return a double representing the data saved into XVariant
 *
 * \note If the data cannot be converted to a double scalar value, then isValid will return false.
 * On the other hand, no error message is set by this method.
 *
 *
 */
double CuVariant::toDouble(bool *ok) const
{
    double v = nan("NaN");
    if(d->type == Double && d->format == Scalar && d->val != NULL)
        v = *((double *)d->val);
    if(ok)
        *ok = d->mIsValid && d->val != NULL;
    return v;
}

long double CuVariant::toLongDouble(bool *ok) const
{
    long double v = nan("NaN");
    if(d->type == LongDouble && d->format == Scalar && d->val != NULL)
        v = *((long double *)d->val);
    if(ok)
        *ok = d->mIsValid && d->val != NULL;
    return v;
}

/** \brief The conversion method that tries to convert the stored data into a long scalar integer
 *
 * @return an int representing the data saved into XVariant
 *
 * \note If the data cannot be converted to a integer scalar value, then isValid will return false.
 * On the other hand, no error message is set by this method.
 *
 */
long int CuVariant::toLongInt(bool *ok) const
{
    long int i = LONG_MIN;
    bool canConvert = d->type == Int || d->type == LongInt;
    if(canConvert && d->format == Scalar && d->val != NULL)
        i = *((long int *)d->val);
    if(ok)
        *ok = d->mIsValid && d->val != NULL;
    return i;
}

int CuVariant::toInt(bool *ok) const
{
    int i = INT_MIN;
    if(d->type == Int && d->format == Scalar && d->val != NULL)
        i = *((int *)d->val);
    if(ok)
        *ok = d->mIsValid && d->val != NULL;
    return i;
}

unsigned int CuVariant::toUInt(bool *ok) const
{
    unsigned int i = 0;
    bool canConvert = d->type == UInt || d->type == Int;
    if(canConvert && d->format == Scalar && d->val != NULL)
        i = *((int *)d->val);
    if(ok)
        *ok = d->mIsValid && d->val != NULL;
    return i;
}

unsigned short CuVariant::toUShortInt(bool *ok) const
{
    unsigned short s = 0;
    bool canConvert = d->type == UShort;
    if(canConvert && d->format == Scalar && d->val != NULL)
        s = *(static_cast<unsigned short *>(d->val));
    if(ok)
        *ok = d->mIsValid && d->val != NULL;
    return s;
}

short CuVariant::toShortInt(bool *ok) const
{
    short s = 0;
    bool canConvert = d->type == Short;
    if(canConvert && d->format == Scalar && d->val != NULL)
        s = *(static_cast<short *>(d->val));
    if(ok)
        *ok = d->mIsValid && d->val != NULL;
    return s;
}

float CuVariant::toFloat(bool *ok) const
{
    float f = 0.0;
    bool canConvert = d->type == Float;
    if(canConvert && d->format == Scalar && d->val != NULL)
        f = *(static_cast<float *>(d->val));
    if(ok)
        *ok = d->mIsValid && d->val != NULL;
    return f;
}

/** \brief The conversion method that tries to convert the stored data into a long unsigned scalar integer
 *
 * @return an int representing the data saved into XVariant
 *
 * \note If the data cannot be converted to a integer scalar value, then isValid will return false.
 * On the other hand, no error message is set by this method.
 *
 */
unsigned long int CuVariant::toULongInt(bool *ok) const
{
    unsigned long int i = -1UL;
    bool canConvert = d->type == UInt || d->type == LongUInt;
    if(canConvert && d->format == Scalar && d->val != NULL)
        i = *((unsigned long int *)d->val);
    if(ok)
        *ok = d->mIsValid && d->val != NULL;
    return i;
}


/** \brief The conversion method that tries to convert the stored data into a scalar boolean
 *
 * @return a bool representing the data saved into XVariant
 *
 * \note If the data cannot be converted to a boolean scalar value, then isValid will return false.
 * On the other hand, no error message is set by this method.
 *
 */
bool CuVariant::toBool(bool *ok) const
{
    bool b = false;
    if(d->type == Boolean && d->format == Scalar&& d->val != NULL)
        b = *((bool *)d->val);
    if(ok)
        *ok = d->mIsValid && (d->val != NULL);
    return b;
}

/** \brief Convert the stored value to a string.
 *
 * This method converts the data stored into an XVariant into a string.
 * For instance, if the XVariant stores a scalar Double data type which value is 0.12
 * then a string containing "0.12" is returned. The  conversion specifier
 * used to convert to double or long double or float data types can be specified in double_format
 * If the CuVariant stores a Vector Double data type with values [10.1, 12.6, 9.1, -5.4]
 * then the string returned will be "10.1,12.6,9.1,-5.4".
 *
 * @param ok if not null, store in ok the result of the conversion.
 *
 * @return a std::string representation of the stored values.
 *
 */
std::string CuVariant::toString(bool *ok, const char *double_format) const
{
    const size_t MAXLEN = 128;
    std::string ret;
    char converted[MAXLEN + 1];

    if(ok)
        *ok = false;
    memset(converted, 0, sizeof(char) * (MAXLEN + 1));
    for(size_t i = 0; i < d->mSize; i++)
    {
        if(i > 0)
            ret += ",";
        if(d->type == String)
            ret += std::string(static_cast<char **>(d->val)[i]);
        else if(d->type == Double)
            snprintf(converted, MAXLEN, double_format, static_cast<double *>(d->val)[i]);
        else if(d->type == Int)
            snprintf(converted, MAXLEN, "%d", static_cast<int *>(d->val)[i]);
        else if(d->type == UInt)
            snprintf(converted, MAXLEN, "%u", static_cast<unsigned int *>(d->val)[i]);
        else if(d->type == LongUInt)
            snprintf(converted, MAXLEN, "%lu", static_cast<long unsigned int *>(d->val)[i]);
        else if(d->type == LongInt)
            snprintf(converted, MAXLEN, "%ld", static_cast<long int *>(d->val)[i]);
        else if(d->type == Short)
            snprintf(converted, MAXLEN, "%d", static_cast<short int *>(d->val)[i]);
        else if(d->type == UShort)
            snprintf(converted, MAXLEN, "%u", static_cast<unsigned short *>(d->val)[i]);
        else if(d->type == Float)
            snprintf(converted, MAXLEN, "%f", static_cast<float *>(d->val)[i]);
        else if(d->type == Boolean)
            static_cast<bool *>(d->val)[i] ? sprintf(converted, "true") : sprintf(converted, "false");
        else if(ok)
        {
            *ok = false;
            perr("CuVariant.toString: error converting data to string: format is %d type is %d",
                 d->format, d->type);
        }

        if(d->type != String)
            ret += std::string(converted);
    }

    return ret;
}

/** \brief The conversion method that tries to convert the stored data into a vector of strings
         *
         * @return a std::vector of string representing the data saved into XVariant
         *
         * \note If the data cannot be converted to a vector of strings, then isValid will return false.
         * On the other hand, no error message is set by this method.
         *
         */
std::vector<std::string> CuVariant::toStringVector(bool *ok) const
{
    std::vector<std::string> ret;
    if(ok)
        *ok = (d->type == String && (d->format == Vector || d->format == Scalar) );

    if(d->type == String && (d->format == Vector || d->format == Scalar) )
    {
        char **str_array = static_cast<char **>(d->val);
        for(size_t i = 0; i < d->mSize; i++)
            ret.push_back(std::string(str_array[i]));
    }
    else
        perr("CuVariant::toStringVector: cannot convert type %d format %d to string vector", d->type, d->format);
    return ret;
}

/** \brief Returns a pointer to a double addressing the start of data.
         *
         * Used with getSize allows to get the stored data in a "C" style.
         *
         * @see getSize
         * @see toLongIntP
         * *
         * \note Check the return value of this method: if null, no data is currently
         *       stored or you are trying to extract a type of data different from the
         *       one memorized in XVariant.
         */
double *CuVariant::toDoubleP() const
{
    return (double *) d->val ;
}

long double *CuVariant::toLongDoubleP() const
{
    return static_cast<long double *>(d->val);
}

/** \brief Returns a pointer to a long unsigned int addressing the start of data.
 *
 * Used with getSize allows to get the stored data in a "C" style.
 *
 * @see getSize
 * @see toDoubleP
 * @see toULongIntP
 *
 * \note Check the return value of this method: if null, no data is currently
 *       stored or you are trying to extract a type of data different from the
 *       one memorized in XVariant.
 *
 * \note shorts and ints are mapped to longs.
 */
unsigned int *CuVariant::toUIntP() const
{
    return static_cast<unsigned int *>( d->val );
}

/** \brief Returns a pointer to a long unsigned int addressing the start of data.
 *
 * Used with getSize allows to get the stored data in a "C" style.
 *
 * @see getSize
 * @see toDoubleP
 * @see toULongIntP
 *
 * \note Check the return value of this method: if null, no data is currently
 *       stored or you are trying to extract a type of data different from the
 *       one memorized in XVariant.
 *
 * \note shorts and ints are mapped to longs.
 */
int *CuVariant::toIntP() const
{
    return static_cast<int *>( d->val );
}

/** \brief Returns a pointer to a long unsigned int addressing the start of data.
         *
         * Used with getSize allows to get the stored data in a "C" style.
         *
         * @see getSize
         * @see toDoubleP
         * @see toULongIntP
         *
         * \note Check the return value of this method: if null, no data is currently
         *       stored or you are trying to extract a type of data different from the
         *       one memorized in XVariant.
         *
         * \note shorts and ints are mapped to longs.
         */
unsigned long int *CuVariant::toULongIntP() const
{
    return (unsigned long int *) d->val ;
}

float *CuVariant::toFloatP() const
{
    return static_cast<float *>(d->val);
}


/** \brief Returns a pointer to an int addressing the start of data.
         *
         * Used with getSize allows to get the stored data in a "C" style.
         *
         * @see getSize
         * @see toDoubleP
         *
         * \note Check the return value of this method: if null, no data is currently
         *       stored or you are trying to extract a type of data different from the
         *       one memorized in XVariant.
         *
         * \note shorts and ints are mapped to longs.
         */
long int *CuVariant::toLongIntP() const
{
    return (long int *) d->val ;
}

short *CuVariant::toShortP() const
{
    return static_cast<short int *>(d->val);
}

unsigned short *CuVariant::toUShortP() const
{
    return static_cast<unsigned short *>(d->val);
}

/** \brief Returns a pointer to a boolean addressing the start of data.
         *
         * Used with getSize allows to get the stored data in a "C" style.
         *
         * @see getSize
         * @see toLongIntP for notes
         * @see toDoubleP for notes
         *
         */
bool *CuVariant::toBoolP() const
{
    return (bool *) d->val ;
}

/** \brief Returns a char pointer addressing the start of data.
         *
         * Used with getSize allows to get the stored data in a "C" style.
         *
         * @see getSize
         * @see toLongIntP for notes
         * @see toDoubleP for notes
         *
         */
char **CuVariant::toCharP() const
{
    return (char **) d->val ;
}

/** \brief Change the storage format to Vector
 *
 */
CuVariant& CuVariant::toVector()
{
    d->format = Vector;
    return *this;
}

/*
 * enum DataFormat { FormatInvalid = -1, Scalar, Vector, Matrix, EndFormatTypes };

    enum DataType { TypeInvalid = -1, Short, UShort, Int, UInt,
                    LongInt, LongUInt, Float, Double,
                    LongDouble, Boolean, String, EndDataTypes };
 */

std::__cxx11::string CuVariant::dataTypeStr(int t) const
{
    const char *v[] = {
        "TypeInvalid", "Short", "UShort", "Int", "UInt",
        "LongInt", "LongUInt", "Float", "Double",
        "LongDouble", "Boolean", "String", "EndDataTypes"
    };
    if(t < EndDataTypes)
        return std::string(v[t]);
    return std::string(std::to_string(t) + " OutOfRange");
}

std::__cxx11::string CuVariant::dataFormatStr(int f) const
{
    const char *v[] = {
        "FormatInvalid", "Scalar", "Vector", "Matrix", "EndFormatTypes"
    };
    if(f < EndFormatTypes)
        return std::string(v[f]);
    return std::string(std::to_string(f) + " OutOfRange");
}

