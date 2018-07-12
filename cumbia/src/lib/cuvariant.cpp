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

/*! \brief deletes read data
 *
 * According to the type, the delete operator is called for the
 * stored value.
 *
 */
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
            delete [] static_cast<double *>(d->val);
        else if(d->type == UInt)
            delete [] static_cast<unsigned int *>(d->val);
        else if(d->type == Int)
            delete [] static_cast<int *>(d->val);
        else if(d->type == Boolean)
            delete [] static_cast<bool *>(d->val);
        else if(d->type == String)
            delete [] static_cast<char *> (d->val);
        else if(d->type == Float)
            delete [] static_cast<float *>(d->val);
        else if(d->type == LongDouble)
            delete [] static_cast<long double *> (d->val);
        else if(d->type == LongInt)
            delete [] static_cast<long int *> (d->val);
        else if(d->type == LongUInt)
            delete [] static_cast<long unsigned int *> (d->val);
        else if(d->type == Short)
            delete [] static_cast<short *> (d->val);
        else if(d->type == UShort)
            delete [] static_cast<unsigned short *> (d->val);
        //        cuprintf("delete_rdata: CuVariant %p deleted d %p d->val %p type %d\n", this, d, d->val, d->dataType);
        d->val = NULL;
    }
}

/*! \brief deletes internal data
 *
 * deletes internal data
 */
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

/*! \brief the class destructor
 *
 * calls cleanup to free all resources used by the object
 */
CuVariant::~CuVariant()
{
    // pdelete("~CuVariant: cleaning up %p", this);
    cleanup();
}

/*! \brief builds a CuVariant holding the specified short integer
 *
 * @param i the value that will be stored by the object as short int
 *
 * Specific conversion method: CuVariant::toShortInt
 */
CuVariant::CuVariant(short i)
{
    d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    init(Scalar, Short);
    from(i);
}

/*! \brief builds a CuVariant holding the specified unsigned short integer
 *
 * @param u the value that will be stored by the object as unsigned short int
 *
 * Specific conversion method: CuVariant::toUShortInt
 */
CuVariant::CuVariant(unsigned short u)
{
    d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    init(Scalar, UShort);
    from(u);
}

/*! \brief builds a CuVariant holding the specified integer
 *
 * @param i the value that will be stored by the object as int
 *
 * Specific conversion method: CuVariant::toInt
 */
CuVariant::CuVariant(int i)
{
    d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    init(Scalar, Int);
    from(i);
}

/*! \brief builds a CuVariant holding the specified unsigned integer
 *
 * @param ui the value that will be stored by the object as unsigned int
 *
 * Specific conversion method: CuVariant::toUInt
 */
CuVariant::CuVariant(unsigned int ui)
{
    d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    init(Scalar, UInt);
    from(ui);
}


/*! \brief builds a CuVariant holding the specified long integer
 *
 * @param li the value that will be stored by the object as long int
 *
 * Specific conversion method: CuVariant::toLongInt
 */
CuVariant::CuVariant(long int li)
{
    d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    init(Scalar, LongInt);
    from(li);
}

/*! \brief builds a CuVariant holding the specified long long integer
 *
 * @param lli the value that will be stored by the object as long long int
 *
 * Specific conversion method: CuVariant::toLongLongInt
 */
CuVariant::CuVariant(long long lli)
{
    d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    init(Scalar, LongLongInt);
    from(lli);
}

/*! \brief builds a CuVariant holding the specified unsigned long integer
 *
 * @param lui the value that will be stored by the object as unsigned long int
 *
 * Specific conversion method: CuVariant::toULongInt
 */
CuVariant::CuVariant(unsigned long int lui)
{
    d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    init(Scalar, LongUInt);
    from(lui);
}

/*! \brief builds a CuVariant holding the specified float
 *
 * @param f the value that will be stored by the object as float
 *
 * Specific conversion method: CuVariant::toFloat
 */
CuVariant::CuVariant(float f)
{
    d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    init(Scalar, Float);
    from(f);
}

/*! \brief builds a CuVariant holding the specified double
 *
 * @param dou the value that will be stored by the object as double
 *
 * Specific conversion method: CuVariant::toDouble
 */
CuVariant::CuVariant(double dou)
{
    d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    init(Scalar, Double);
    from(dou);
}

/*! \brief builds a CuVariant holding the specified long double
 *
 * @param ld the value that will be stored by the object as long double
 *
 * Specific conversion method: CuVariant::toLongDouble
 */
CuVariant::CuVariant(long double ld)
{
    d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    init(Scalar, LongDouble);
    from(ld);
}

/*! \brief builds a CuVariant holding the specified boolean value
 *
 * @param b the value that will be stored by the object as bool
 *
 * Specific conversion method: CuVariant::toBool
 */
CuVariant::CuVariant(bool b)
{
    d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    init(Scalar, Boolean);
    from(b);
}

/*! \brief builds a CuVariant holding the specified std::string
 *
 * @param s the value that will be stored by the object as string
 *
 * Specific conversion method: CuVariant::toString
 */
CuVariant::CuVariant(const std::string&  s)
{
    d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    init(Scalar, String);
    from_std_string(s);
}

/*! \brief builds a CuVariant holding the specified constant char string
 *
 * @param s the value that will be stored by the object as string
 *
 * Specific conversion method: CuVariant::toString
 */
CuVariant::CuVariant(const char *s)
{
    d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    init(Scalar, String);
    from_std_string(std::string(s));
}

/*! \brief builds a CuVariant storing the void * pointer passed as argument
 *
 * @param ptr a generic void * pointer.
 *
 * Specific conversion method: CuVariant::toVoidPtr
 *
 * \par Note
 * ptr is not deleted when CuVariant is deleted.
 */
CuVariant::CuVariant(void *ptr)
{
    d = new CuVariantPrivate();
    init(Scalar, VoidPtr);
    d->mSize = 1;
    d->mIsNull = false;
    d->mIsValid = true;
    d->val = ptr;
}

/*! \brief builds a CuVariant holding the specified vector of double
 *
 * @param vd the value that will be stored by the object as vector of double elements
 *
 * Specific conversion method: CuVariant::toDoubleVector
 */
CuVariant::CuVariant(const std::vector<double> &vd)
{
    d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    init(Vector, Double);
    from(vd);
}

/*! \brief builds a CuVariant holding the specified vector of long double
 *
 * @param vd the value that will be stored by the object as vector of long double elements
 *
 * Specific conversion method: CuVariant::toLongDoubleVector
 */
CuVariant::CuVariant(const std::vector<long double> &vd)
{
    d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    init(Vector, LongDouble);
    from(vd);
}

/*! \brief builds a CuVariant holding the specified vector of booleans
 *
 * @param vb the value that will be stored by the object as vector of bool elements
 *
 * Specific conversion method: CuVariant::toBoolVector
 */
CuVariant::CuVariant(const std::vector<bool> &vb)
{
    d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    init(Vector, Boolean);
    from(vb);
}

/*! \brief builds a CuVariant holding the specified vector of std::string
 *
 * @param vs the value that will be stored by the object as vector of string elements
 *
 * Specific conversion method: CuVariant::toStringVector
 */
CuVariant::CuVariant(const std::vector<std::string> &vs)
{
    d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    init(Vector, String);
    from(vs);
}

/*! \brief builds a CuVariant holding the specified vector of short integers
 *
 * @param si the value that will be stored by the object as vector of short integer elements
 *
 * Specific conversion method: CuVariant::toShortVector
 */
CuVariant::CuVariant(const std::vector<short> &si)
{
    d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    init(Vector, Short);
    from(si);
}

/*! \brief builds a CuVariant holding the specified vector of unsigned short integers
 *
 * @param si the value that will be stored by the object as vector of unsigned short integer elements
 *
 * Specific conversion method: CuVariant::toUShortVector
 */
CuVariant::CuVariant(const std::vector<unsigned short> &si)
{
    d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    init(Vector, UShort);
    from(si);
}

/*! \brief builds a CuVariant holding the specified vector of integers
 *
 * @param vi the value that will be stored by the object as vector of integer elements
 *
 * Specific conversion method: CuVariant::toIntVector
 */
CuVariant::CuVariant(const std::vector<int> &vi)
{
    d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    init(Vector, Int);
    from(vi);
}

/*! \brief builds a CuVariant holding the specified vector of unsigned integers
 *
 * @param vi the value that will be stored by the object as vector of unsigned integer elements
 *
 * Specific conversion method: CuVariant::toUIntVector
 */
CuVariant::CuVariant(const std::vector<unsigned int> &vi)
{
    d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    init(Vector, UInt);
    from(vi);
}

/*! \brief builds a CuVariant holding the specified vector of long integers
 *
 * @param li the value that will be stored by the object as vector of long integer elements
 *
 * Specific conversion method: CuVariant::toLongIntVector
 */
CuVariant::CuVariant(const std::vector<long int> &li)
{
    d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    init(Vector, LongInt);
    from(li);
}

/*! \brief builds a CuVariant holding the specified vector of unsigned long integers
 *
 * @param li the value that will be stored by the object as vector of unsigned long integer elements
 *
 * Specific conversion method: CuVariant::toULongIntVector
 */
CuVariant::CuVariant(const std::vector<unsigned long> &lui)
{
    d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    init(Vector, LongUInt);
    from(lui);
}

/*! \brief builds a CuVariant holding the specified vector of float
 *
 * @param li the value that will be stored by the object as vector of float elements
 *
 * Specific conversion method: CuVariant::toFloatVector
 */
CuVariant::CuVariant(const std::vector<float> &vf)
{
    d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    init(Vector, Float);
    from(vf);
}

/** \brief Creates an empty (invalid) CuVariant
 *
 * The CuVariant created is not *valid* and *null*.
 *
 * \li CuVariant::isValid would return false
 * \li CuVariant::isNull would return true
 * \li CuVariant::getFormat would return CuVariant::FormatInvalid
 * \li CuVariant::getType would return CuVariant::TypeInvalid
 */
CuVariant::CuVariant()
{
    d = new CuVariantPrivate();  /* allocates CuVariantDataInfo */
    d->format = FormatInvalid;
    d->type = TypeInvalid;
}

/** \brief copy constructor
 *
 * Create a new variant initialized from the values of the other parameter
 *
 * @param other the CuVariant to copy from.
 *
 */
CuVariant::CuVariant(const CuVariant &other)
{
    build_from(other);
}

/*! \brief move constructor
 *
 * @param other the other CuVariant data is moved from
 *
 * C++ 11 move constructor for CuVariant
 */
CuVariant::CuVariant(CuVariant &&other)
{
    /* no new d here! */
    d = other.d;
    other.d = NULL; /* don't delete */
}

/*!	\brief assignment operator, copies data from another CuVariant
 *
 * @param other CuVariant to assign from
 */
CuVariant & CuVariant::operator=(const CuVariant& other)
{
    if(this != &other)
    {
        cleanup();
        build_from(other);
    }
    return *this;
}

/*! \brief move	assignment operator, moves data from another source
 *
 * @param other CuVariant to move from
 */
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

/*! \brief equality relational operator. Returns true if this CuVariant equals another one
 *
 * @param other CuVariant to compare this to
 * @return true this and other contain the same data type, format, have the
 *         same isValid flag and the value is the same
 *
 * The method first checks if getFormat, getSize, getType, isNull, isValid
 * return values coincide.
 * Then *memcmp* is used to compare the value of this and the other CuVariant.
 */
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
                    LongDouble, Boolean, String, VoidPtr, EndDataTypes };
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
    case VoidPtr:
        return d->val == other.d->val;
        break;
    default:
        break;
    }
    return false;
}

/*! \brief returns the opposite result of the *equality* operator
 *
 * @return the negation of CuVariant::operator ==
 */
bool CuVariant::operator !=(const CuVariant &other) const
{
    return !this->operator ==(other);
}

/*
 * builds this variant from another one copying the contents
 */
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
    else if(d->type == CuVariant::LongUInt)
    {
        unsigned long int *uli =  new unsigned long int[d->mSize];
        for(size_t i = 0; i < d->mSize; i++)
            uli[i] = other.toULongIntP()[i];
        d->val = uli;
    }
    else if(d->type == CuVariant::VoidPtr) {
        d->val = other.d->val;
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

/** \brief get the format of the data stored
 *
 * @return the DataFormat (CuVariant::Vector, CuVariant::Scalar, CuVariant::Matrix)
 * @see getType
 */
CuVariant::DataFormat CuVariant::getFormat() const
{
    return static_cast<CuVariant::DataFormat>(d->format);
}

/** \brief Returns the DataType stored by CuVariant
 *
 * @return one of the CuVariant::DataType enumeration values
 * @see  getFormat
 */
CuVariant::DataType CuVariant::getType() const
{
    return static_cast<CuVariant::DataType>(d->type);
}

/** \brief Returns whether the data stored by CuVariant is valid
 *
 * @return true the data contained by CuVariant is valid
 * @return false the data contained by CuVariant is not valid (see getError)
 *
 * @see getError
 * @see isNull
 *
 * \note
 * isValid may return true if data is NULL. In other words, NULL values
 * are deemed valid.
 *
 */
bool CuVariant::isValid() const
{
    return d->mIsValid;
}

/** \brief Returns whether the the data stored by CuVariant is NULL or not
 *
 * @return true the data contained by CuVariant is NULL
 * @return false the data contained by CuVariant is not NULL (see isValid)
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

/** \brief Returns the size of the data stored by the CuVariant
 *
 * @return the size of the data stored by the CuVariant. This method is useful to
 *         know the number of elements, if CuVariant holds a vector
 */
size_t CuVariant::getSize() const
{
    return d->mSize;
}

/*! \brief returns true if the stored data is an integer number
 *
 * @return true if the data type is an integer, false otherwise
 *
 * \note returns true if CuVariant::DataType is one of
 * \li Short
 * \li UShort
 * \li Int
 * \li UInt
 * \li LongInt
 * \li LongUInt
 */
bool CuVariant::isInteger() const
{
    return d->type == Short || d->type == UShort
            || d->type ==  Int|| d->type ==  UInt ||
            d->type == LongInt || d->type ==  LongUInt ||
            d->type == LongLongInt;
}

/*! \brief returns true if the stored data is a floating point number
 *
 * @return true if the data type is a floating point number,
 *         false otherwise
 *
 * \note returns true if CuVariant::DataType is one of
 * \li Double
 * \li LongDouble
 * \li Float
 */
bool CuVariant::isFloatingPoint() const
{
    return d->type == Double || d->type == LongDouble
            || d->type == Float;
}

/*! \brief returns true if the data stored is a void * pointer
 *
 */
bool CuVariant::isVoidPtr() const
{
    return d->type == VoidPtr;
}

/*
 * init CuVariant with the given data format and type
 * - size is put to 0
 * - isNull property is set to true
 * - isValid property is set to true if format and data type are valid
 */
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

/*
 * build a CuVariant holding a vector of strings
 */
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

/*
 * given a std::string, build a string type CuVariant
 * - size is set to 1
 * - isNull is set to false
 */
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

/*
 * template method to initialize a CuVariant from a template type
 * - size is set to 1
 * - isNull is set to false
 */
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

/** \brief convert the stored data into a vector of double
 *
 * @return std::vector<double> representation of the stored data
 *
 * Compatible constructor: CuVariant::CuVariant(const std::vector<double> &vd)
 *
 */
std::vector<double> CuVariant::toDoubleVector() const
{
    double *d_val;
    d_val = static_cast<double *>(d->val);
    std::vector<double> dvalues(d_val, d_val + d->mSize);
    return dvalues;
}

/** \brief convert the stored data into a vector of long double
 *
 * @return std::vector<long double> representation of the stored data
 *
 * Compatible constructor: CuVariant::CuVariant(const std::vector<long double> &vd)
 */
std::vector<long double> CuVariant::toLongDoubleVector() const
{
    long double *d_val;
    d_val = (long double *) d->val;
    std::vector<long double> ldvalues(d_val, d_val + d->mSize);
    return ldvalues;
}

/** \brief convert the stored data into a vector of float
 *
 * @return std::vector<float> representation of the stored data
 *
 * Compatible constructor: CuVariant::CuVariant(const std::vector<float> &vf)
 */
std::vector<float> CuVariant::toFloatVector() const
{
    float *fval;
    fval =  static_cast<float*>(d->val);
    std::vector<float> fvalues(fval, fval + d->mSize);
    return fvalues;
}

/** \brief convert the stored data into a vector of integers
 *
 * @return std::vector<int> representation of the stored data
 *
 * Compatible constructor: CuVariant::CuVariant(const std::vector<int> &vi)
 */
std::vector<int> CuVariant::toIntVector() const
{
    int *i_val = (int *) d->val;
    std::vector<int> ivalues(i_val, i_val + d->mSize);
    return ivalues;
}

/** \brief convert the stored data into a vector of unsigned integers
 *
 * @return std::vector<unsigned int> representation of the stored data
 *
 * Compatible constructor: CuVariant::CuVariant(const std::vector<unsigned int> &vi)
 */
std::vector<unsigned int> CuVariant::toUIntVector() const
{
    unsigned int *i_val = (unsigned int *) d->val;
    std::vector<unsigned int> uivalues(i_val, i_val + d->mSize);
    return uivalues;
}

/** \brief convert the stored data into a vector of unsigned integers
 *
 * @return std::vector<unsigned int> representation of the stored data
 *
 * Compatible constructor: CuVariant::CuVariant(const std::vector<unsigned long> &lui)
 */
std::vector<unsigned long int> CuVariant::toULongIntVector() const
{
    unsigned long int *i_val = static_cast<unsigned long int *>(d->val);
    std::vector<unsigned long int> ivalues(i_val, i_val + d->mSize);
    return ivalues;
}

/** \brief convert the stored data into a vector of long integers
 *
 * @return std::vector<long int> representation of the stored data
 *
 * Compatible constructor: CuVariant::CuVariant(const std::vector<long> &li)
 */
std::vector<long int> CuVariant::toLongIntVector() const
{
    long int *i_val = (long int *) d->val;
    std::vector<long int> ivalues(i_val, i_val + d->mSize);
    return ivalues;
}

/** \brief convert the stored data into a vector of booleans
 *
 * @return std::vector<bool> representation of the stored data
 *
 * Compatible constructor: CuVariant::CuVariant(const std::vector<bool> &vb)
 */
std::vector<bool> CuVariant::toBoolVector() const
{
    bool *b_val = (bool *) d->val;
    std::vector<bool> bvalues(b_val, b_val + d->mSize);
    return bvalues;
}

/** \brief convert the stored data into a vector of short integers
 *
 * @return std::vector<short> representation of the stored data
 *
 * Compatible constructor: CuVariant::CuVariant(const std::vector<short> &si)
 */
std::vector<short> CuVariant::toShortVector() const
{
    short *v_s = static_cast<short *>(d->val);
    std::vector<short> svals(v_s, v_s + d->mSize);
    return svals;
}

/** \brief convert the stored data into a vector of unsigned short integers
 *
 * @return std::vector<unsigned short> representation of the stored data
 *
 * Compatible constructor: CuVariant::CuVariant(const std::vector<unsigned short> &si)
 */
std::vector<unsigned short> CuVariant::toUShortVector() const
{
    unsigned short *v_us = static_cast<unsigned short *>(d->val);
    std::vector<unsigned short> usvals(v_us, v_us + d->mSize);
    return usvals;
}

/** \brief convert the stored data into a double scalar
 *
 * @param *ok a pointer to a bool. If not null, its value will be set to true
 *        if the conversion is successful, false otherwise (wrong data type,
 *        format, invalid CuVariant or NULL value)
 * @return the double value held in this object, or *NaN* if either the data type
 *         or format is wrong, the CuVariant is not valid or the value is NULL
 */
double CuVariant::toDouble(bool *ok) const
{
    double v = nan("NaN");
    bool can_convert = (d->type == Double && d->format == Scalar && d->val != NULL && d->mIsValid);
    if(can_convert)
        v = *((double *)d->val);
    if(ok)
        *ok = can_convert;
    return v;
}

/** \brief convert the stored data into a long double scalar
 *
 * @param *ok a pointer to a bool. If not null, its value will be set to true
 *        if the conversion is successful, false otherwise (wrong data type,
 *        format, invalid CuVariant or NULL value)
 * @return the long double value held in this object, or *NaN* if either the data type
 *         or format is wrong, the CuVariant is not valid or the value is NULL
 */
long double CuVariant::toLongDouble(bool *ok) const
{
    long double v = nanl("NaN");
    bool can_convert = (d->type == LongDouble && d->format == Scalar && d->val != NULL && d->mIsValid);
    if(can_convert)
        v = *((long double *)d->val);
    if(ok)
        *ok = can_convert;
    return v;
}

/** \brief convert the stored data into a scalar long int
 *
 * @param *ok a pointer to a bool. If not null, its value will be set to true
 *        if the conversion is successful, false otherwise (wrong data type,
 *        format, invalid CuVariant or NULL value)
 * @return the long int value held in this object, or LONG_MIN if either the data type
 *         or format is wrong, the CuVariant is not valid or the value is NULL
 *
 * \note since it is legal to convert an int to long int, both DataType::Int
 *       and DataType::LongInt are valid
 */
long int CuVariant::toLongInt(bool *ok) const
{
    long int i = LONG_MIN;
    bool canConvert = (d->type == Int || d->type == LongInt)  && d->format == Scalar && d->val != NULL && d->mIsValid;
    if(canConvert && d->format == Scalar && d->val != NULL && d->type == Int)
        i = static_cast<long int> ( *(static_cast<int *>(d->val)) );
    else if(canConvert && d->type == LongInt)
        i = *(static_cast<long int*>(d->val));
    if(ok)
        *ok = canConvert;
    return i;
}

long long CuVariant::toLongLongInt(bool *ok) const
{
    long long int i = LONG_LONG_MIN;
    bool canConvert = (d->type == Int || d->type == LongInt)  && d->format == Scalar && d->val != NULL && d->mIsValid;
    if(canConvert && d->format == Scalar && d->val != NULL && d->type == Int)
        i = static_cast<long long int> ( *(static_cast<int *>(d->val)) );
    else if(canConvert && d->type == LongInt)
        i = static_cast<long long int> (*(static_cast<long int*>(d->val)));
    else if(canConvert && d->type == LongLongInt)
        return *(static_cast<long long int*>(d->val));
    if(ok)
        *ok = canConvert;
    return i;
}

/** \brief convert the stored data into a scalar integer
 *
 * @param *ok a pointer to a bool. If not null, its value will be set to true
 *        if the conversion is successful, false otherwise (wrong data type,
 *        format, invalid CuVariant or NULL value)
 * @return the int value held in this object, or INT_MIN if either the data type
 *         or format is wrong, the CuVariant is not valid or the value is NULL
 *
 * \note since it is legal to convert an short int to int, both DataType::Int
 *       and DataType::Short are valid
 */
int CuVariant::toInt(bool *ok) const
{
    int i = INT_MIN;
    bool canConvert = (d->type == Int || d->type == Short)  && d->format == Scalar && d->val != NULL && d->mIsValid;
    if(canConvert && d->type == Int)
        i = *(static_cast<int *>(d->val) );
    else if(canConvert && d->type == Short)
        i = static_cast<int>( *(static_cast<short int *>(d->val)));
    if(ok)
        *ok = canConvert;
    return i;
}

/** \brief convert the stored data into a scalar unsigned integer
 *
 * @param *ok a pointer to a bool. If not null, its value will be set to true
 *        if the conversion is successful, false otherwise (wrong data type,
 *        format, invalid CuVariant or NULL value)
 * @return the unsigned int value held in this object, or UINT_MAX if either the data type
 *         or format is wrong, the CuVariant is not valid or the value is NULL
 *
 * \note since it is legal to convert an unsigned short int to an unsigned int, both DataType::UShort
 *       and DataType::UInt are valid
 */
unsigned int CuVariant::toUInt(bool *ok) const
{
   unsigned int i = UINT_MAX;
   bool can_convert = (d->format == Scalar && d->val != NULL  && d->mIsValid && (d->type == UInt || d->type == UShort) );
   if(can_convert && d->type == UInt)
        i = *(static_cast<unsigned int *>(d->val) );
   else if(can_convert && d->type == UShort)
       i = static_cast<unsigned int>( *(static_cast<unsigned short *>(d->val)) );
    if(ok)
        *ok = can_convert;
    return i;
}

/** \brief convert the stored data into a scalar unsigned short
 *
 * @param *ok a pointer to a bool. If not null, its value will be set to true
 *        if the conversion is successful, false otherwise (wrong data type,
 *        format, invalid CuVariant or NULL value)
 * @return the unsigned short value held in this object, or USHRT_MAX if either the data type
 *         or format is wrong, the CuVariant is not valid or the value is NULL
 *
 */
unsigned short CuVariant::toUShortInt(bool *ok) const
{
    unsigned short s = USHRT_MAX;
    bool canConvert = d->type == UShort && d->format == Scalar && d->val != NULL && d->mIsValid;
    if(canConvert )
        s = *(static_cast<unsigned short *>(d->val));
    if(ok)
        *ok = canConvert;
    return s;
}

/** \brief convert the stored data into a scalar signed short
 *
 * @param *ok a pointer to a bool. If not null, its value will be set to true
 *        if the conversion is successful, false otherwise (wrong data type,
 *        format, invalid CuVariant or NULL value)
 * @return the signed short value held in this object, or SHRT_MIN if either the data type
 *         or format is wrong, the CuVariant is not valid or the value is NULL
 *
 */
short CuVariant::toShortInt(bool *ok) const
{
    short s = SHRT_MIN;
    bool canConvert = d->type == Short && d->format == Scalar && d->val != NULL && d->mIsValid;
    if(canConvert)
        s = *(static_cast<short *>(d->val));
    if(ok)
        *ok = canConvert;
    return s;
}

/** \brief convert the stored data into a scalar floating point number
 *
 * @param *ok a pointer to a bool. If not null, its value will be set to true
 *        if the conversion is successful, false otherwise (wrong data type,
 *        format, invalid CuVariant or NULL value)
 * @return the float value held in this object, or NaN (as returned by nanf) if either the data type
 *         or format is wrong, the CuVariant is not valid or the value is NULL
 *
 */
float CuVariant::toFloat(bool *ok) const
{
    float f = nanf("NaN");
    bool canConvert = d->type == Float && d->format == Scalar && d->val != NULL && d->mIsValid;
    if(canConvert)
        f = *(static_cast<float *>(d->val));
    if(ok)
        *ok = canConvert;
    return f;
}

/** \brief convert the stored data into a scalar unsigned long number
 *
 * @param *ok a pointer to a bool. If not null, its value will be set to true
 *        if the conversion is successful, false otherwise (wrong data type,
 *        format, invalid CuVariant or NULL value)
 * @return the unsigned long value held in this object, or ULONG_MAX if either the data type
 *         or format is wrong, the CuVariant is not valid or the value is NULL
 *
 * \note since it is legal to convert an unsigned int to an unsigned long int, both DataType::LongUInt
 *       and DataType::UInt are valid
 */
unsigned long int CuVariant::toULongInt(bool *ok) const
{
    unsigned long int i = ULONG_MAX;
    bool canConvert = (d->type == UInt || d->type == LongUInt)  && d->format == Scalar && d->val != NULL && d->mIsValid;
    if(canConvert && d->type == UInt)
        i = static_cast<unsigned long int> ( *(static_cast<unsigned int* >(d->val) ) );
    else if(canConvert && d->type == LongUInt)
        i = *(static_cast<unsigned  long int* >(d->val) );
    if(ok)
        *ok = canConvert;
    return i;
}


/** \brief convert the stored data into a scalar boolean
 *
 * @param *ok a pointer to a bool. If not null, its value will be set to true
 *        if the conversion is successful, false otherwise (wrong data type,
 *        format, invalid CuVariant or NULL value)
 * @return the bool value held in this object, or ULONG_MAX if either the data type
 *         or format is wrong, the CuVariant is not valid or the value is NULL
 */
bool CuVariant::toBool(bool *ok) const
{
    bool b = false;
    bool can_convert = (d->type == Boolean && d->format == Scalar && d->val != NULL && d->mIsValid);
    if(can_convert)
        b = *(static_cast<bool *> (d->val) );
    if(ok)
        *ok = can_convert;
    return b;
}

/** \brief Convert the stored value to a string representation.
 *
 * This method converts the data stored by CuVariant into a string, using
 * the *snprintf* function with the given format or a default format specifier chosen
 * on the basis of the data type.
 *
 * \par Default conversion specifier
 * \li double: "%f"
 * \li long double: "%Lf"
 * \li float: "%f"
 * \li int: "%d"
 * \li long int: "%ld"
 * \li unsigned int: "%u"
 * \li long unsigned int: "%lu"
 * \li short: "%hd"
 * \li unsigned short: "%hu"
 *
 * \par Examples
 *
 * \li if the CuVariant holds a scalar Double data type with value 0.12,
 *     a string containing "0.12" is returned.
 *
 * \li if the CuVariant stores a Vector Double data type with values [10.1, 12.6, 9.1, -5.4]
 *     then the returned string will be "10.1,12.6,9.1,-5.4".
 *
 * The  conversion specifier can be given through the format parameter of this method
 *
 * @param ok if not null, store in ok the result of the conversion.
 * @param format a const char specifying the desired format to pass to *snprintf*
 *
 * @return a std::string representation of the stored values.
 *
 */
std::string CuVariant::toString(bool *ok, const char *format) const
{
    const size_t MAXLEN = 128;
    std::string ret;
    char converted[MAXLEN + 1];
    bool success = true;
    memset(converted, 0, sizeof(char) * (MAXLEN + 1));
    for(size_t i = 0; i < d->mSize && success; i++)
    {
        if(i > 0)
            ret += ",";
        if(d->type == String)
            ret += std::string(static_cast<char **>(d->val)[i]);
        else if(d->type == Double)
            snprintf(converted, MAXLEN, "%f", static_cast<double *>(d->val)[i]);
        else if(d->type == LongDouble)
            snprintf(converted, MAXLEN, "%Lf", static_cast<long double *>(d->val)[i]);
        else if(d->type == Int)
            snprintf(converted, MAXLEN, "%d", static_cast<int *>(d->val)[i]);
        else if(d->type == UInt)
            snprintf(converted, MAXLEN, "%u", static_cast<unsigned int *>(d->val)[i]);
        else if(d->type == LongUInt)
            snprintf(converted, MAXLEN, "%lu", static_cast<long unsigned int *>(d->val)[i]);
        else if(d->type == LongInt)
            snprintf(converted, MAXLEN, "%ld", static_cast<long int *>(d->val)[i]);
        else if(d->type == Short)
            snprintf(converted, MAXLEN, "%hd", static_cast<short int *>(d->val)[i]);
        else if(d->type == UShort)
            snprintf(converted, MAXLEN, "%hu", static_cast<unsigned short *>(d->val)[i]);
        else if(d->type == Float)
            snprintf(converted, MAXLEN, "%f", static_cast<float *>(d->val)[i]);
        else if(d->type == Boolean)
            static_cast<bool *>(d->val)[i] ? sprintf(converted, "true") : sprintf(converted, "false");
        else if(d->type == VoidPtr)
            snprintf(converted, MAXLEN, "%p", d->val);
        else
        {
            success = false;
            ret = "";
            perr("CuVariant.toString: error converting data to string: format is %d type is %d",
                 d->format, d->type);
        }

        if(d->type != String)
            ret += std::string(converted);
    }

    if(ok)
        *ok = success;
    return ret;
}

/** \brief convert the stored data into a vector of strings
 *
 * @param *ok a pointer to a bool. If not null, its value will be set to true
 *        if the conversion is successful, false otherwise (wrong data type,
 *        format, invalid CuVariant or NULL value)
 *
 * @return a std::vector of std::string representing the data saved into CuVariant
 *
 * \note conversion is successful if the CuVariant::DataType is CuVariant::String and the
 * CuVariant::DataFormat is *either* CuVariant::Vector *or* CuVariant::Scalar
 *
 */
std::vector<std::string> CuVariant::toStringVector(bool *ok) const
{
    bool success = true;
    bool native_type = (d->type == String && (d->format == Vector || d->format == Scalar) );
    std::vector<std::string> ret;
    if(native_type)
    {
        char **str_array = static_cast<char **>(d->val);
        for(size_t i = 0; i < d->mSize; i++)
            ret.push_back(std::string(str_array[i]));
    }
    else if(d->format == Vector || d->format == Scalar) {
        const size_t MAXLEN = 128;
        char converted[MAXLEN + 1];

        for(size_t i = 0; i < d->mSize && success; i++) // while success is true
        {
            memset(converted, 0, sizeof(char) * (MAXLEN + 1));  // clear string

            if(d->type == String) // directly push back the native data
                ret.push_back(std::string(static_cast<char **>(d->val)[i]));
            else if(d->type == Double)
                snprintf(converted, MAXLEN, "%f", static_cast<double *>(d->val)[i]);
            else if(d->type == LongDouble)
                snprintf(converted, MAXLEN, "%Lf", static_cast<long double *>(d->val)[i]);
            else if(d->type == Int)
                snprintf(converted, MAXLEN, "%d", static_cast<int *>(d->val)[i]);
            else if(d->type == UInt)
                snprintf(converted, MAXLEN, "%u", static_cast<unsigned int *>(d->val)[i]);
            else if(d->type == LongUInt)
                snprintf(converted, MAXLEN, "%lu", static_cast<long unsigned int *>(d->val)[i]);
            else if(d->type == LongInt)
                snprintf(converted, MAXLEN, "%ld", static_cast<long int *>(d->val)[i]);
            else if(d->type == Short)
                snprintf(converted, MAXLEN, "%hd", static_cast<short int *>(d->val)[i]);
            else if(d->type == UShort)
                snprintf(converted, MAXLEN, "%hu", static_cast<unsigned short *>(d->val)[i]);
            else if(d->type == Float)
                snprintf(converted, MAXLEN, "%f", static_cast<float *>(d->val)[i]);
            else if(d->type == Boolean)
                static_cast<bool *>(d->val)[i] ? sprintf(converted, "true") : sprintf(converted, "false");
            else
            {
                success = false;
                perr("CuVariant.toStringVector: error converting data to string vector: format is %s type is %s",
                     dataFormatStr(d->format).c_str(), dataTypeStr(d->type).c_str());
            }
            if(success && strlen(converted) > 0)
                ret.push_back(std::string(converted));
        }
    }
    else
        success = false;

    if(ok) {
        *ok = success;
    }
    if(!success) {
        perr("CuVariant::toStringVector: cannot convert type %s format %s to string vector",
             dataFormatStr(d->format).c_str(), dataTypeStr(d->type).c_str());
    }
    return ret;
}

/** \brief Returns the pointer to the data stored as double, or NULL if no data is stored
 *         or if the data type is wrong
 *
 * Combined with getSize allows to get the stored data in a "C" style (as scalar, if
 * size is 1, or array).
 *
 * @return pointer to the data held as double, NULL if no data is stored or the
 *         CuVariant::DataType is not CuVariant::Double
 *
 * @see getSize
 * @see getData
 *
 */
double *CuVariant::toDoubleP() const
{
    if(d->type == CuVariant::Double)
        return static_cast<double *> (d->val);
    return NULL;
}

/** \brief Returns the pointer to the data stored as long double, or NULL
 *         if no data is stored or if the data type is wrong
 *
 * Combined with getSize allows to get the stored data in a "C" style (as scalar, if
 * size is 1, or array)
 *
 * @return pointer to the data held as double, NULL if no data is stored or the
 *         CuVariant::DataType is not CuVariant::LongDouble
 *
 * @see getSize
 * @see getData
 */
long double *CuVariant::toLongDoubleP() const
{
    if(d->type == CuVariant::LongDouble)
        return static_cast<long double *> (d->val);
    return NULL;
}

/** \brief Returns the pointer to the data stored as unsigned int, or NULL
 *         if no data is stored or if the data type is wrong
 *
 * Combined with getSize allows to get the stored data in a "C" style (as scalar, if
 * size is 1, or array)
 *
 * @return pointer to the data held as double, NULL if no data is stored or the
 *         CuVariant::DataType is not CuVariant::UInt
 *
 * @see getSize
 * @see getData
 */
unsigned int *CuVariant::toUIntP() const
{
    if(d->type == CuVariant::UInt) return static_cast<unsigned int *>( d->val );
    return NULL;
}

/** \brief Returns the pointer to the data stored as int, or NULL
 *         if no data is stored or if the data type is wrong
 *
 * Combined with getSize allows to get the stored data in a "C" style (as scalar, if
 * size is 1, or array)
 *
 * @return pointer to the data held as int, NULL if no data is stored or the
 *         CuVariant::DataType is not CuVariant::Int
 *
 * @see getSize
 * @see getData
 */
int *CuVariant::toIntP() const
{
    if(d->type == CuVariant::Int) return static_cast<int *>( d->val );
    return NULL;
}

/** \brief Returns the pointer to the data stored as long int, or NULL
 *         if no data is stored or if the data type is wrong
 *
 * Combined with getSize allows to get the stored data in a "C" style (as scalar, if
 * size is 1, or array)
 *
 * @return pointer to the data held as int, NULL if no data is stored or the
 *         CuVariant::DataType is not CuVariant::LongInt
 *
 * @see getSize
 * @see getData
 */
long int *CuVariant::toLongIntP() const
{
    if(d->type == CuVariant::LongInt)
        return static_cast<long int *>(d->val);
    return NULL;
}

long long int *CuVariant::toLongLongIntP() const
{
    if(d->type == CuVariant::LongLongInt)
        return static_cast<long long int *>(d->val);
    return NULL;
}

/** \brief Returns the pointer to the data stored as unsigned long int, or NULL
 *         if no data is stored or if the data type is wrong
 *
 * Combined with getSize allows to get the stored data in a "C" style (as scalar, if
 * size is 1, or array)
 *
 * @return pointer to the data held as int, NULL if no data is stored or the
 *         CuVariant::DataType is not CuVariant::LongUInt
 *
 * @see getSize
 * @see getData
 */
unsigned long int *CuVariant::toULongIntP() const
{
    if(d->type == CuVariant::LongUInt)
        return static_cast<unsigned long int *>(d->val);
    return NULL;
}

/** \brief Returns the pointer to the data stored as float, or NULL
 *         if no data is stored or if the data type is wrong
 *
 * Combined with getSize allows to get the stored data in a "C" style (as scalar, if
 * size is 1, or array)
 *
 * @return pointer to the data held as int, NULL if no data is stored or the
 *         CuVariant::DataType is not CuVariant::Float
 *
 * @see getSize
 * @see getData
 */
float *CuVariant::toFloatP() const
{
    if(d->type == CuVariant::Float)
        return static_cast<float *>(d->val);
    return NULL;
}

/** \brief Returns the pointer to the data stored as short int, or NULL
 *         if no data is stored or if the data type is wrong
 *
 * Combined with getSize allows to get the stored data in a "C" style (as scalar, if
 * size is 1, or array)
 *
 * @return pointer to the data held as int, NULL if no data is stored or the
 *         CuVariant::DataType is not CuVariant::Short
 *
 * @see getSize
 * @see getData
 */
short *CuVariant::toShortP() const
{
    if(d->type == CuVariant::Short)
        return static_cast<short int *>(d->val);
    return NULL;
}

/** \brief Returns the pointer to the data stored as unsigned short int, or NULL
 *         if no data is stored or if the data type is wrong
 *
 * Combined with getSize allows to get the stored data in a "C" style (as scalar, if
 * size is 1, or array)
 *
 * @return pointer to the data held as int, NULL if no data is stored or the
 *         CuVariant::DataType is not CuVariant::UShort
 *
 * @see getSize
 * @see getData
 */
unsigned short *CuVariant::toUShortP() const
{
    if(d->type == CuVariant::UShort)
        return static_cast<unsigned short *>(d->val);
    return NULL;
}

/** \brief Returns the pointer to the data stored as a boolean, or NULL
 *         if no data is stored or if the data type is wrong
 *
 * Combined with getSize allows to get the stored data in a "C" style (as scalar, if
 * size is 1, or array)
 *
 * @return pointer to the data held as int, NULL if no data is stored or the
 *         CuVariant::DataType is not CuVariant::Bool
 *
 * @see getSize
 * @see getData
 */
bool *CuVariant::toBoolP() const
{
    if(d->type == CuVariant::Boolean)
        return static_cast<bool *> (d->val );
    return NULL;
}

/** \brief Returns the pointer to the data stored as a char**, or NULL
 *         if no data is stored or if the data type is wrong
 *
 * Combined with getSize allows to get the stored data in a "C" style (as scalar, if
 * size is 1, or array)
 *
 * @return pointer to the data held as int, NULL if no data is stored or the
 *         CuVariant::DataType is not CuVariant::String
 *
 * @see getSize
 * @see getData
 */
char **CuVariant::toCharP() const
{
    if(d->type == CuVariant::String)
        return (char **) d->val ;
    return NULL;
}

void *CuVariant::toVoidP() const
{
    if(d->type == CuVariant::VoidPtr) {
        return d->val;
    }
    return NULL;
}

/** \brief Change the storage format to Vector
 *
 * modify the CuVariant::DataFormat to vector, so that data can
 * be interpreted as vector
 */
CuVariant& CuVariant::toVector()
{
    d->format = Vector;
    return *this;
}

/*! \brief string representation of the CuVariant::DataType
 *
 * @param t a value from CuVariant::DataType
 * @return a std::string representation of the input CuVariant::DataType
 *
 */
std::string CuVariant::dataTypeStr(int t) const
{
    const char *v[] = {
        "TypeInvalid", "Short", "UShort", "Int", "UInt",
        "LongInt", "LongUInt", "Float", "Double",
        "LongDouble", "Boolean", "String", "VoidPtr", "EndDataTypes"
    };
    if(t >= 0 && t < EndDataTypes)
        return std::string(v[t]);
    return std::string(std::to_string(t) + " OutOfRange");
}

/*! \brief string representation of CuVariant::DataFormat
 *
 * @param f a value from the CuVariant::DataFormat enum
 * @return a std::string representation of the given data format
 */
std::string CuVariant::dataFormatStr(int f) const
{
    const char *v[] = {
        "FormatInvalid", "Scalar", "Vector", "Matrix", "EndFormatTypes"
    };
    if(f < EndFormatTypes)
        return std::string(v[f]);
    return std::string(std::to_string(f) + " OutOfRange");
}

