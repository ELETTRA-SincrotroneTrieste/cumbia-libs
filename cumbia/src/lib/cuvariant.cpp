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
#include <atomic>

CuVariantPrivate::CuVariantPrivate(const CuVariantPrivate &other) {
    format  = other.format;
    type = other.type;
    mSize = other.mSize;
    mIsValid = other.mIsValid;
    mIsNull = other.mIsNull;
    _r.store(1);
    
    switch(type) {
    case CuVariant::Double: {
        if(format == CuVariant::Scalar || format == CuVariant::Vector) {
            double *vd = new double[mSize];
            for(size_t i = 0; i < mSize; i++)
                vd[i] =  static_cast<double *> (other.val)[i];
            val = vd;
        }
        else if(format == CuVariant::Matrix )
            val = static_cast<CuMatrix <double > *> (other.val)->clone();
    } break;
    case CuVariant::Int:
    {
        if(format == CuVariant::Scalar || format == CuVariant::Vector) {
            int *vi =  new int[mSize];
            for(size_t i = 0; i < mSize; i++)
                vi[i] = static_cast<int *> (other.val)[i];
            val = vi;
        }
        else if(format == CuVariant::Matrix )
            val = static_cast<CuMatrix <int > *> (other.val)->clone();
    } break;
    case CuVariant::UInt:  {
        if(format == CuVariant::Scalar || format == CuVariant::Vector) {
            unsigned int *vi =  new unsigned int[mSize];
            for(size_t i = 0; i < mSize; i++)
                vi[i] = static_cast<unsigned int *> (other.val)[i];
            val = vi;
        }
        else if(format == CuVariant::Matrix )
            val = static_cast<CuMatrix <unsigned int > *> (other.val)->clone();
    } break;
    case CuVariant::LongInt:  {
        if(format == CuVariant::Scalar || format == CuVariant::Vector) {
            long int *vi =  new long int[mSize];
            for(size_t i = 0; i < mSize; i++)
                vi[i] = static_cast<long int *> (other.val)[i];
            val = vi;
        }
        else if(format == CuVariant::Matrix )
            val = static_cast<CuMatrix <long int > *> (other.val)->clone();
    } break;
    case CuVariant::LongUInt:  {
        if(format == CuVariant::Scalar || format == CuVariant::Vector) {
            unsigned long int *uli =  new unsigned long int[mSize];
            for(size_t i = 0; i < mSize; i++)
                uli[i] = static_cast<unsigned long int *> (other.val)[i];
            val = uli;
        }
        else if(format == CuVariant::Matrix )
            val = static_cast<CuMatrix <unsigned long int > *> (other.val)->clone();
    } break;
        
        
    case CuVariant::LongLongInt:  {
        if(format == CuVariant::Scalar || format == CuVariant::Vector) {
            long long int *vi =  new long long int[mSize];
            for(size_t i = 0; i < mSize; i++)
                vi[i] = static_cast<long long int *> (other.val)[i];
            val = vi;
        }
        else if(format == CuVariant::Matrix )
            val = static_cast<CuMatrix <long long int > *> (other.val)->clone();
    } break;
    case CuVariant::LongLongUInt:  {
        if(format == CuVariant::Scalar || format == CuVariant::Vector) {
            unsigned long long int *uli =  new unsigned long long int[mSize];
            for(size_t i = 0; i < mSize; i++)
                uli[i] = static_cast<unsigned long long int *> (other.val)[i];
            val = uli;
        }
        else if(format == CuVariant::Matrix )
            val = static_cast<CuMatrix <unsigned long long int > *> (other.val)->clone();
    } break;
        
        
    case CuVariant::VoidPtr: {
        val = other.val;
    } break;
    case CuVariant::Boolean: {
        if(format == CuVariant::Scalar || format == CuVariant::Vector) {
            bool *vb = new bool[mSize];
            for(size_t i = 0; i < mSize; i++)
                vb[i] = static_cast<bool *> (other.val)[i];
            val = vb;
        }
        else if(format == CuVariant::Matrix )
            val = static_cast<CuMatrix <bool > *> (other.val)->clone();
    } break;
    case CuVariant::Short:  {
        if(format == CuVariant::Scalar || format == CuVariant::Vector) {
            short *vs = new short[mSize];
            for(size_t i = 0; i < mSize; i++)
                vs[i] = static_cast<short *> (other.val)[i];
            val = vs;
        }
        else if(format == CuVariant::Matrix )
            val = static_cast<CuMatrix <short > *> (other.val)->clone();
    } break;
    case CuVariant::UShort: {
        if(format == CuVariant::Scalar || format == CuVariant::Vector) {
            unsigned short *vus = new unsigned short[mSize];
            for(size_t i = 0; i < mSize; i++)
                vus[i] = static_cast<unsigned short *> (other.val)[i];
            val = vus;
        }
        else if(format == CuVariant::Matrix )
            val = static_cast<CuMatrix <unsigned short > *> (other.val)->clone();
    } break;
    case CuVariant::Char:  {
        if(format == CuVariant::Scalar || format == CuVariant::Vector) {
            char *vc = new char[mSize];
            for(size_t i = 0; i < mSize; i++)
                vc[i] = static_cast<char *> (other.val)[i];
            val = vc;
        }
        else if(format == CuVariant::Matrix )
            val = static_cast<CuMatrix <char > *> (other.val)->clone();
    } break;
    case CuVariant::UChar: {
        if(format == CuVariant::Scalar || format == CuVariant::Vector) {
            unsigned char *vuch = new unsigned char[mSize];
            for(size_t i = 0; i < mSize; i++)
                vuch[i] = static_cast<unsigned char *> (other.val)[i];
            val = vuch;
        }
        else if(format == CuVariant::Matrix )
            val = static_cast<CuMatrix <unsigned char > *> (other.val)->clone();
    } break;
    case CuVariant::Float: {
        if(format == CuVariant::Scalar || format == CuVariant::Vector) {
            float *vf = new float[mSize];
            for(size_t i = 0; i < mSize; i++)
                vf[i] = static_cast<float *> (other.val)[i];
            val = vf;
        }
        else if(format == CuVariant::Matrix )
            val = static_cast<CuMatrix <float > *> (other.val)->clone();
    } break;
    case CuVariant::String: {
        if(format == CuVariant::Scalar || format == CuVariant::Vector) {
            char *other_s;
            char **str = new char *[mSize];
            size_t len;
            for(size_t i = 0; i < mSize; i++)
            {
                other_s =   static_cast<char **>(other.val)[i];
                if(other_s != nullptr)
                {
                    len = strlen(other_s);
                    str[i] = new char[len + 1];
                    memset(str[i], 0, sizeof(char) * (len + 1));
                    strncpy(str[i], other_s, len + 1);
                }
                else
                    str[i] = nullptr;
            }
            val = str;
        }
        else if(format == CuVariant::Matrix ) {
            val = CuMatrix<std::string>::from_string_matrix(*(static_cast<CuMatrix <std::string > *> (other.val)));
        }
    } break;
    case CuVariant::FormatInvalid:
        break;
    default:
        perr("CuVariantPrivate [copy constructor]: unsupported data type %d", type);
        break;
    } // switch d->type
}

CuVariantPrivate::CuVariantPrivate()
{
    val = 0; // NULL
    mIsValid = false;
    mIsNull = true;
    mSize = 0;
    _r.store(1);
}

CuVariantPrivate::~CuVariantPrivate() {
    
}

/*! \brief deletes read data
 *
 * According to the type, the delete operator is called for the
 * stored value.
 *
 */
void CuVariant::m_delete_rdata()
{
    if(_d->val != NULL)
    {
        /*
         * num DataType { TypeInvalid = -1, Short, UShort, Int, UInt,
         *           LongInt, LongUInt, Float, Double,
         *           LongDouble, Boolean, String, EndDataTypes };
         */
        if(_d->type == Double)
            delete [] static_cast<double *>(_d->val);
        else if(_d->type == UInt)
            delete [] static_cast<unsigned int *>(_d->val);
        else if(_d->type == Int)
            delete [] static_cast<int *>(_d->val);
        else if(_d->type == Boolean)
            delete [] static_cast<bool *>(_d->val);
        else if(_d->type == String)
            delete [] static_cast<char *> (_d->val);
        else if(_d->type == Float)
            delete [] static_cast<float *>(_d->val);
        else if(_d->type == LongDouble)
            delete [] static_cast<long double *> (_d->val);
        else if(_d->type == LongInt)
            delete [] static_cast<long int *> (_d->val);
        else if(_d->type == LongUInt)
            delete [] static_cast<long unsigned int *> (_d->val);
        else if(_d->type == LongLongInt)
            delete [] static_cast<long long int *> (_d->val);
        else if(_d->type == LongLongUInt)
            delete [] static_cast<unsigned long long int *> (_d->val);
        else if(_d->type == Short)
            delete [] static_cast<short *> (_d->val);
        else if(_d->type == UShort)
            delete [] static_cast<unsigned short *> (_d->val);
        else if(_d->type == UChar)
            delete [] static_cast<unsigned char *> (_d->val);
        else if(_d->type == Char)
            delete [] static_cast<char *> (_d->val);
        //        cuprintf("delete_rdata: CuVariant %p deleted d %p d->val %p type %d\n", this, d, d->val, d->dataType);
        _d->val = NULL;
    }
}

/*! \brief deletes internal data
 *
 * deletes internal data
 */
void CuVariant::m_cleanup()
{
    if(_d != NULL)
    {
        if(_d->mSize > 0 && _d->type == String && _d->val != NULL)
        {
            char **ssi = (char **) _d->val;
            for(size_t i = 0; i < _d->mSize; i++)
            {
                char *si = (char *) ssi[i];
                delete[] si;
            }
        }
        m_delete_rdata();
        delete _d;
        _d = NULL;
    }
}

void CuVariant::m_detach() {
    if(_d->load() > 1) {
        _d->unref();
        _d = new CuVariantPrivate(*_d); // sets ref=1  TEST: increases vcp
    }
}

/*! \brief the class destructor
 *
 * calls cleanup to free all resources used by the object
 */
CuVariant::~CuVariant() {
    if(_d && _d->unref() == 1) {
        // pdelete("~CuVariant: cleaning up %p", this);
        m_cleanup(); // deletes d
    }
}

/*! \brief builds a CuVariant holding the specified short integer
 *
 * @param i the value that will be stored by the object as short int
 *
 * Specific conversion method: CuVariant::toShortInt
 */
CuVariant::CuVariant(short i)
{
    _d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    m_init(Scalar, Short);
    m_from(i);
}

CuVariant::CuVariant(char c) {
    _d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    m_init(Scalar, Char);
    m_from(c);
}

CuVariant::CuVariant(unsigned char uc) {
    _d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    m_init(Scalar, UChar);
    m_from(uc);
}

/*! \brief builds a CuVariant holding the specified unsigned short integer
 *
 * @param u the value that will be stored by the object as unsigned short int
 *
 * Specific conversion method: CuVariant::toUShortInt
 */
CuVariant::CuVariant(unsigned short u) {
    _d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    m_init(Scalar, UShort);
    m_from(u);
}

/*! \brief builds a CuVariant holding the specified integer
 *
 * @param i the value that will be stored by the object as int
 *
 * Specific conversion method: CuVariant::toInt
 */
CuVariant::CuVariant(int i) {
    _d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    m_init(Scalar, Int);
    m_from(i);
}

/*! \brief builds a CuVariant holding the specified unsigned integer
 *
 * @param ui the value that will be stored by the object as unsigned int
 *
 * Specific conversion method: CuVariant::toUInt
 */
CuVariant::CuVariant(unsigned int ui) {
    _d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    m_init(Scalar, UInt);
    m_from(ui);
}


/*! \brief builds a CuVariant holding the specified long integer
 *
 * @param li the value that will be stored by the object as long int
 *
 * Specific conversion method: CuVariant::toLongInt
 */
CuVariant::CuVariant(long int li) {
    _d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    m_init(Scalar, LongInt);
    m_from(li);
}

/*! \brief builds a CuVariant holding the specified long long integer
 *
 * @param lli the value that will be stored by the object as long long int
 *
 * Specific conversion method: CuVariant::toLongLongInt
 */
CuVariant::CuVariant(long long lli) {
    _d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    m_init(Scalar, LongLongInt);
    m_from(lli);
}

/*! \brief builds a CuVariant holding the specified unsigned long integer
 *
 * @param lui the value that will be stored by the object as unsigned long int
 *
 * Specific conversion method: CuVariant::toULongInt
 */
CuVariant::CuVariant(unsigned long int lui) {
    _d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    m_init(Scalar, LongUInt);
    m_from(lui);
}

CuVariant::CuVariant(unsigned long long ului) {
    _d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    m_init(Scalar, LongLongUInt);
    m_from(ului);
}

/*! \brief builds a CuVariant holding the specified float
 *
 * @param f the value that will be stored by the object as float
 *
 * Specific conversion method: CuVariant::toFloat
 */
CuVariant::CuVariant(float f) {
    _d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    m_init(Scalar, Float);
    m_from(f);
}

/*! \brief builds a CuVariant holding the specified double
 *
 * @param dou the value that will be stored by the object as double
 *
 * Specific conversion method: CuVariant::toDouble
 */
CuVariant::CuVariant(double dou) {
    _d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    m_init(Scalar, Double);
    m_from(dou);
}

/*! \brief builds a CuVariant holding the specified long double
 *
 * @param ld the value that will be stored by the object as long double
 *
 * Specific conversion method: CuVariant::toLongDouble
 */
CuVariant::CuVariant(long double ld) {
    _d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    m_init(Scalar, LongDouble);
    m_from(ld);
}

/*! \brief builds a CuVariant holding the specified boolean value
 *
 * @param b the value that will be stored by the object as bool
 *
 * Specific conversion method: CuVariant::toBool
 */
CuVariant::CuVariant(bool b) {
    _d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    m_init(Scalar, Boolean);
    m_from(b);
}

/*! \brief builds a CuVariant holding the specified std::string
 *
 * @param s the value that will be stored by the object as string
 *
 * Specific conversion method: CuVariant::toString
 */
CuVariant::CuVariant(const std::string&  s) {
    _d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    m_init(Scalar, String);
    m_from_std_string(s);
}

/*! \brief builds a CuVariant holding the specified constant char string
 *
 * @param s the value that will be stored by the object as string
 *
 * Specific conversion method: CuVariant::toString
 */
CuVariant::CuVariant(const char *s) {
    _d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    m_init(Scalar, String);
    m_from_std_string(std::string(s));
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
CuVariant::CuVariant(void *ptr) {
    _d = new CuVariantPrivate();
    m_init(Scalar, VoidPtr);
    _d->mSize = 1;
    _d->mIsNull = false;
    _d->mIsValid = true;
    _d->val = ptr;
}

/*! \brief builds a CuVariant holding the specified vector of double
 *
 * @param vd the value that will be stored by the object as vector of double elements
 *
 * Specific conversion method: CuVariant::toDoubleVector
 */
CuVariant::CuVariant(const std::vector<double> &vd) {
    _d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    m_init(Vector, Double);
    m_from(vd);
}

/*! \brief builds a CuVariant holding the specified vector of long double
 *
 * @param vd the value that will be stored by the object as vector of long double elements
 *
 * Specific conversion method: CuVariant::toLongDoubleVector
 */
CuVariant::CuVariant(const std::vector<long double> &vd) {
    _d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    m_init(Vector, LongDouble);
    m_from(vd);
}

/*! \brief builds a CuVariant holding the specified vector of booleans
 *
 * @param vb the value that will be stored by the object as vector of bool elements
 *
 * Specific conversion method: CuVariant::toBoolVector
 */
CuVariant::CuVariant(const std::vector<bool> &vb) {
    _d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    m_init(Vector, Boolean);
    m_from(vb);
}

/*! \brief builds a CuVariant holding the specified vector of std::string
 *
 * @param vs the value that will be stored by the object as vector of string elements
 *
 * Specific conversion method: CuVariant::toStringVector
 */
CuVariant::CuVariant(const std::vector<std::string> &vs) {
    _d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    m_init(Vector, String);
    m_from(vs);
}

/*!
 * \brief vector of char flavor
 * \param vc std vector of char
 */
CuVariant::CuVariant(const std::vector<char> &vc) {
    _d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    m_init(Vector, Char);
    m_from(vc);
}

/*!
 * \brief vector of unsigned char flavor
 * \param vc std vector of unsigned char
 */
CuVariant::CuVariant(const std::vector<unsigned char> &vc) {
    _d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    m_init(Vector, UChar);
    m_from(vc);
}

/*! \brief builds a CuVariant holding the specified vector of short integers
 *
 * @param si the value that will be stored by the object as vector of short integer elements
 *
 * Specific conversion method: CuVariant::toShortVector
 */
CuVariant::CuVariant(const std::vector<short> &si) {
    _d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    m_init(Vector, Short);
    m_from(si);
}

/*! \brief builds a CuVariant holding the specified vector of unsigned short integers
 *
 * @param si the value that will be stored by the object as vector of unsigned short integer elements
 *
 * Specific conversion method: CuVariant::toUShortVector
 */
CuVariant::CuVariant(const std::vector<unsigned short> &si) {
    _d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    m_init(Vector, UShort);
    m_from(si);
}

/*! \brief builds a CuVariant holding the specified vector of integers
 *
 * @param vi the value that will be stored by the object as vector of integer elements
 *
 * Specific conversion method: CuVariant::toIntVector
 */
CuVariant::CuVariant(const std::vector<int> &vi) {
    _d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    m_init(Vector, Int);
    m_from(vi);
}

/*! \brief builds a CuVariant holding the specified vector of unsigned integers
 *
 * @param vi the value that will be stored by the object as vector of unsigned integer elements
 *
 * Specific conversion method: CuVariant::toUIntVector
 */
CuVariant::CuVariant(const std::vector<unsigned int> &vi) {
    _d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    m_init(Vector, UInt);
    m_from(vi);
}

/*! \brief builds a CuVariant holding the specified vector of long integers
 *
 * @param li the value that will be stored by the object as vector of long integer elements
 *
 * Specific conversion method: CuVariant::toLongIntVector
 */
CuVariant::CuVariant(const std::vector<long int> &li) {
    _d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    m_init(Vector, LongInt);
    m_from(li);
}

/*!
 * \brief builds a CuVariant holding the specified vector of long long integers
 * \param lli vector of long long integers
 * Specific conversion method: CuVariant::toLongLongIntVector
 */
CuVariant::CuVariant(const std::vector<long long> &lli) {
    _d = new CuVariantPrivate();
    m_init(Vector, LongLongInt);
    m_from(lli);
}

/*! \brief builds a CuVariant holding the specified vector of unsigned long integers
 *
 * @param li the value that will be stored by the object as vector of unsigned long integer elements
 *
 * Specific conversion method: CuVariant::toULongIntVector
 */
CuVariant::CuVariant(const std::vector<unsigned long> &lui) {
    _d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    m_init(Vector, LongUInt);
    m_from(lui);
}

CuVariant::CuVariant(const std::vector<unsigned long long> &llui) {
    _d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    m_init(Vector, LongLongUInt);
    m_from(llui);
}

/*! \brief builds a CuVariant holding the specified vector of float
 *
 * @param li the value that will be stored by the object as vector of float elements
 *
 * Specific conversion method: CuVariant::toFloatVector
 */
CuVariant::CuVariant(const std::vector<float> &vf) {
    _d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    m_init(Vector, Float);
    m_from(vf);
}


CuVariant::CuVariant(const std::vector<unsigned char> &m, size_t dimx, size_t dimy) {
    _d = new CuVariantPrivate();
    m_init(Matrix, UChar);
    m_v_to_matrix(m, dimx, dimy);
}

CuVariant::CuVariant(const std::vector<char> &m, size_t dimx, size_t dimy) {
    _d = new CuVariantPrivate();
    m_init(Matrix, Char);
    m_v_to_matrix(m, dimx, dimy);
}

CuVariant::CuVariant(const std::vector<short> &i, size_t dimx, size_t dimy) {
    _d = new CuVariantPrivate();
    m_init(Matrix, Short);
    m_v_to_matrix(i, dimx, dimy);
}

CuVariant::CuVariant(const std::vector<unsigned short> &si, size_t dimx, size_t dimy) {
    _d = new CuVariantPrivate();
    m_init(Matrix, UShort);
    m_v_to_matrix(si, dimx, dimy);
}

CuVariant::CuVariant(const std::vector<int> &vi, size_t dimx, size_t dimy) {
    _d = new CuVariantPrivate();
    m_init(Matrix, Int);
    m_v_to_matrix(vi, dimx, dimy);
}

CuVariant::CuVariant(const std::vector<unsigned int> &ui, size_t dimx, size_t dimy){
    _d = new CuVariantPrivate();
    m_init(Matrix, UInt);
    m_v_to_matrix(ui, dimx, dimy);
}

CuVariant::CuVariant(const std::vector<long> &li, size_t dimx, size_t dimy) {
    _d = new CuVariantPrivate();
    m_init(Matrix, LongInt);
    m_v_to_matrix(li, dimx, dimy);
}

CuVariant::CuVariant(const std::vector<long long> &lli, size_t dimx, size_t dimy) {
    _d = new CuVariantPrivate();
    m_init(Matrix, LongLongInt);
    m_v_to_matrix(lli, dimx, dimy);
}

CuVariant::CuVariant(const std::vector<unsigned long> &lui, size_t dimx, size_t dimy) {
    _d = new CuVariantPrivate();
    m_init(Matrix, LongUInt);
    m_v_to_matrix(lui, dimx, dimy);
}

CuVariant::CuVariant(const std::vector<unsigned long long> &llui, size_t dimx, size_t dimy) {
    _d = new CuVariantPrivate();
    m_init(Matrix, LongLongUInt);
    m_v_to_matrix(llui, dimx, dimy);
}

CuVariant::CuVariant(const std::vector<float> &vf, size_t dimx, size_t dimy)
{
    _d = new CuVariantPrivate();
    m_init(Matrix, Float);
    m_v_to_matrix(vf, dimx, dimy);
}

CuVariant::CuVariant(const std::vector<double> &vd, size_t dimx, size_t dimy)
{
    _d = new CuVariantPrivate();
    m_init(Matrix, Double);
    m_v_to_matrix(vd, dimx, dimy);
}

CuVariant::CuVariant(const std::vector<long double> &vld, size_t dimx, size_t dimy) {
    _d = new CuVariantPrivate();
    m_init(Matrix, LongDouble);
    m_v_to_matrix(vld, dimx, dimy);
}

CuVariant::CuVariant(const std::vector<bool> &vb, size_t dimx, size_t dimy) {
    _d = new CuVariantPrivate();
    m_init(Matrix, Boolean);
    m_v_to_matrix(vb, dimx, dimy);
}

CuVariant::CuVariant(const std::vector<std::string> &vs, size_t dimx, size_t dimy) {
    _d = new CuVariantPrivate();
    m_init(Matrix, String);
    m_v_to_string_matrix(vs, dimx, dimy);
}


/*! \brief builds a CuVariant holding the specified vector of void* pointers
 *
 * @param li the value that will be stored by the object as vector of void* elements
 *
 * Specific conversion method: CuVariant::toFloatVector
 */
CuVariant::CuVariant(const std::vector<void *> &vptr) {
    _d = new CuVariantPrivate(); /* allocates CuVariantDataInfo */
    m_init(Vector, VoidPtr);
    m_from(vptr);
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
CuVariant::CuVariant() {
    _d = new CuVariantPrivate();  /* allocates CuVariantDataInfo */
    _d->format = FormatInvalid;
    _d->type = TypeInvalid;
}

/** \brief copy constructor
 *
 * Create a new variant initialized from the values of the other parameter
 *
 * @param other the CuVariant to copy from.
 *
 */
CuVariant::CuVariant(const CuVariant &other) {
    _d = other._d;
    _d->ref();
    //  build_from(other); // before shared data
}

/*! \brief move constructor
 *
 * @param other the other CuVariant data is moved from
 *
 * C++ 11 move constructor for CuVariant
 */
CuVariant::CuVariant(CuVariant &&other) {
    /* no new d here! */
    _d = other._d;
    other._d = nullptr; /* don't delete */
}

/*!	\brief assignment operator, copies data from another CuVariant
 *
 * @param other CuVariant to assign from
 */
CuVariant & CuVariant::operator=(const CuVariant& other) {
    if(this != &other)
    {
        other._d->ref();
        if(this->_d->unref() == 1) {
            m_cleanup();
        }
        _d = other._d; // share
        // >> build_from(other); // before shared data
    }
    return *this;
}

/*! \brief move	assignment operator, moves data from another source
 *
 * @param other CuVariant to move from
 */
CuVariant &CuVariant::operator=(CuVariant &&other) {
    if(this != &other)
    {
        if(_d && _d->unref() == 1) {
            m_cleanup();
        }
        _d = other._d;
        other._d = NULL; /* don't delete */
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
    if(! (other.getFormat() ==  this->getFormat() && other.getSize() ==  this->getSize() &&
          other.getType() ==  this->getType() && other.isNull() ==  this->isNull() &&
          other.isValid() ==  this->isValid() ) )
        return false;
    /* one d->val is null and the other not */
    if( (other._d->val == nullptr &&  this->_d->val != nullptr) || (other._d->val != nullptr &&  this->_d->val == nullptr) )
        return false;
    if(other._d->val == nullptr && _d->val == nullptr)
        return true;
    
    char **v_str = NULL, **other_v_str = NULL;
    /*
    * enum DataType { TypeInvalid = -1, Short, UShort, Int, UInt,
                    LongInt, LongUInt, Double,
                    LongDouble, Boolean, String, VoidPtr, EndDataTypes };
    */
    switch (_d->type) {
    case Short:
        return memcmp(other._d->val,  this->_d->val, sizeof(short) * _d->mSize) == 0;
    case UShort:
        return memcmp(other._d->val,  this->_d->val, sizeof(unsigned short) * _d->mSize) == 0;
    case Int:
        return memcmp(other._d->val,  this->_d->val, sizeof(int) * _d->mSize) == 0;
    case UInt:
        return memcmp(other._d->val,  this->_d->val, sizeof(unsigned int) * _d->mSize) == 0;
    case LongInt:
        return memcmp(other._d->val,  this->_d->val, sizeof(long int) * _d->mSize) == 0;
    case LongLongInt:
        return memcmp(other._d->val,  this->_d->val, sizeof(long long int) * _d->mSize) == 0;
    case LongUInt:
        return memcmp(other._d->val,  this->_d->val, sizeof(long unsigned int) * _d->mSize) == 0;
    case LongLongUInt:
        return memcmp(other._d->val,  this->_d->val, sizeof(long long unsigned int) * _d->mSize) == 0;
    case Float:
        return memcmp(other._d->val,  this->_d->val, sizeof(float) * _d->mSize) == 0;
    case Double:
        return memcmp(other._d->val,  this->_d->val, sizeof(double) * _d->mSize) == 0;
    case LongDouble:
        return memcmp(other._d->val,  this->_d->val, sizeof(long double) * _d->mSize) == 0;
    case Boolean:
        return memcmp(other._d->val,  this->_d->val, sizeof(bool) * _d->mSize) == 0;
    case UChar:
        return memcmp(other._d->val,  this->_d->val, sizeof(unsigned char) * _d->mSize) == 0;
    case Char:
        return memcmp(other._d->val,  this->_d->val, sizeof(char) * _d->mSize) == 0;

    case String:
        v_str = static_cast<char **>(_d->val);
        other_v_str = static_cast<char **>(other._d->val);
        if(_d->mSize == 1)
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
        return _d->val == other._d->val;
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
bool CuVariant::operator !=(const CuVariant &other) const {
    return !this->operator ==(other);
}

/*
 * builds this variant from another one copying the contents
 */
void CuVariant::build_from(const CuVariant& other) {
    _d = new CuVariantPrivate(*(other._d));
}

/** \brief get the format of the data stored
 *
 * @return the DataFormat (CuVariant::Vector, CuVariant::Scalar, CuVariant::Matrix)
 * @see getType
 */
CuVariant::DataFormat CuVariant::getFormat() const
{
    return static_cast<CuVariant::DataFormat>(_d->format);
}

/** \brief Returns the DataType stored by CuVariant
 *
 * @return one of the CuVariant::DataType enumeration values
 * @see  getFormat
 */
CuVariant::DataType CuVariant::getType() const
{
    return static_cast<CuVariant::DataType>(_d->type);
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
    return _d->mIsValid;
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
    return _d->mIsNull;
}

/** \brief Returns the size of the data stored by the CuVariant
 *
 * @return the size of the data stored by the CuVariant. This method is useful to
 *         know the number of elements, if CuVariant holds a vector
 */
size_t CuVariant::getSize() const
{
    return _d->mSize;
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
    return _d->type == Short || _d->type == UShort
            || _d->type ==  Int|| _d->type ==  UInt ||
            _d->type == LongInt || _d->type ==  LongUInt ||
            _d->type == LongLongInt || _d->type == UChar ||
            _d->type == LongLongUInt || _d->type == Char;
}

bool CuVariant::isUnsignedType() const {
    return _d->type == UShort || _d->type ==  UInt ||
            _d->type ==  LongUInt || _d->type == UChar ||
            _d->type == LongLongUInt;
}

bool CuVariant::isSignedType() const {
    return _d->type == Short || _d->type == Int || _d->type == LongInt || _d->type == LongLongInt || _d->type == Char;
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
    return _d->type == Double || _d->type == LongDouble
            || _d->type == Float;
}

/*! \brief returns true if the data stored is a void * pointer
 *
 */
bool CuVariant::isVoidPtr() const
{
    return _d->type == VoidPtr;
}

/*
 * init CuVariant with the given data format and type
 * - size is put to 0
 * - isNull property is set to true
 * - isValid property is set to true if format and data type are valid
 */
void CuVariant::m_init(DataFormat df, DataType dt) {
    _d->mIsValid = (dt > TypeInvalid && dt < EndDataTypes) && (df > FormatInvalid && df < EndFormatTypes);
    _d->mSize = 0;
    _d->format = df;
    _d->type = dt;
    _d->mIsNull = true;
}


template<typename T>
void CuVariant::m_from(const std::vector<T> &v) {
    if(!_d->mIsValid || _d->type == String)
        perr("CuVariant::from <vector T>: invalid data type or format. Have you called init first??");
    else  {
        _d->val = NULL;
        _d->mSize = v.size();
        
        _d->val = (T *) new T[_d->mSize];
        for(size_t i = 0; i < _d->mSize; i++)
            static_cast<T *>(_d->val)[i] = (T) v[i];
        
        _d->mIsNull = false;
    }
}

template<typename T>
void CuVariant::m_v_to_matrix(const std::vector<T> &v, size_t dimx, size_t dim_y) {
    if(!_d->mIsValid || _d->type == String)
        perr("CuVariant::from_matrix <T>: invalid data type or format. Have you called init first??");
    else {
        CuMatrix<T> *m = new CuMatrix<T>(v, dimx, dim_y);
        _d->val = m ; // static_cast<CuMatrix <T>* >(m);
    }
}

void CuVariant::m_v_to_string_matrix(const std::vector<std::string> &vs, size_t dimx, size_t dim_y) {
    if(_d->mIsValid && _d->type == String) {
        CuMatrix<std::string> *m = new CuMatrix<std::string>(vs, dimx, dim_y);
        _d->val = m; /*static_cast<CuMatrix <std::string>* >(m);*/
    }
    else {
        perr("CuVariant::from_matrix <T>: invalid data type or format. Have you called init first??");
    }
}

/*
 * build a CuVariant holding a vector of strings
 */
void CuVariant::m_from(const std::vector<std::string> &s) {
    _d->val = NULL;
    if(!_d->mIsValid || _d->type != String)
        perr("CuVariant::from <vector std::string>: invalid data type or format. Have you called init first??");
    else /* strings need alloc and strcpy */
    {
        _d->mSize = s.size();
        char **str_array = new char*[_d->mSize];
        for(size_t i = 0; i < _d->mSize; i++)
        {
            str_array[i] = new char[s[i].size() + 1];
            strncpy(str_array[i], s[i].c_str(), s[i].size() + 1);
        }
        _d->mIsNull = false;
        _d->val = str_array;
    }
}

/*
 * given a std::string, build a string type CuVariant
 * - size is set to 1
 * - isNull is set to false
 */
void CuVariant::m_from_std_string(const std::string &s) {
    _d->mSize = 1;
    size_t size = strlen(s.c_str()) + 1;
    char **str = new char*[_d->mSize];
    str[0] = new char[size];
    strncpy(str[0], s.c_str(), sizeof(char) * size);
    _d->mIsNull = false;
    _d->val = str;
}

/*
 * template method to initialize a CuVariant from a template type
 * - size is set to 1
 * - isNull is set to false
 */
template<typename T>
void CuVariant::m_from(T value) {
    if(!_d->mIsValid || _d->type == String)
        perr("CuVariant::from <T>: invalid data type or format. Have you called init first??");
    else
    {
        _d->mSize = 1;
        _d->val = (T *) new T[_d->mSize];
        *(static_cast<T *> (_d->val) ) = (T) value;
        _d->mIsNull = false;
    }
    
}

/** \brief convert the stored data into a vector of double
 *
 * @return std::vector<double> representation of the stored data
 *
 * Compatible constructor: CuVariant::CuVariant(const std::vector<double> &vd)
 *
 */
std::vector<double> CuVariant::toDoubleVector() const {
    double *d_val;
    d_val = static_cast<double *>(_d->val);
    std::vector<double> dvalues(d_val, d_val + _d->mSize);
    return dvalues;
}

/** \brief convert the stored data into a vector of long double
 *
 * @return std::vector<long double> representation of the stored data
 *
 * Compatible constructor: CuVariant::CuVariant(const std::vector<long double> &vd)
 */
std::vector<long double> CuVariant::toLongDoubleVector() const {
    long double *d_val;
    d_val = (long double *) _d->val;
    std::vector<long double> ldvalues(d_val, d_val + _d->mSize);
    return ldvalues;
}

/** \brief convert the stored data into a vector of float
 *
 * @return std::vector<float> representation of the stored data
 *
 * Compatible constructor: CuVariant::CuVariant(const std::vector<float> &vf)
 */
std::vector<float> CuVariant::toFloatVector() const {
    float *fval;
    fval =  static_cast<float*>(_d->val);
    std::vector<float> fvalues(fval, fval + _d->mSize);
    return fvalues;
}

/** \brief convert the stored data into a vector of integers
 *
 * @return std::vector<int> representation of the stored data
 *
 * Compatible constructor: CuVariant::CuVariant(const std::vector<int> &vi)
 */
std::vector<int> CuVariant::toIntVector() const {
    int *i_val = (int *) _d->val;
    std::vector<int> ivalues(i_val, i_val + _d->mSize);
    return ivalues;
}

/** \brief convert the stored data into a vector of unsigned integers
 *
 * @return std::vector<unsigned int> representation of the stored data
 *
 * Compatible constructor: CuVariant::CuVariant(const std::vector<unsigned int> &vi)
 */
std::vector<unsigned int> CuVariant::toUIntVector() const{
    unsigned int *i_val = (unsigned int *) _d->val;
    std::vector<unsigned int> uivalues(i_val, i_val + _d->mSize);
    return uivalues;
}

/** \brief convert the stored data into a vector of unsigned long long integers
 *
 * @return std::vector<unsigned long long> representation of the stored data
 *
 * Compatible constructor: CuVariant::CuVariant(const std::vector<unsigned long long> &vi)
 */
std::vector<unsigned long long> CuVariant::toLongLongUIntVector() const {
    unsigned long long int *u_ll_i_val = static_cast<unsigned long long int *>(_d->val);
    std::vector<unsigned long long int> u_ll_ivalues(u_ll_i_val, u_ll_i_val + _d->mSize);
    return u_ll_ivalues;
}

/** \brief convert the stored data into a vector of unsigned integers
 *
 * @return std::vector<unsigned int> representation of the stored data
 *
 * Compatible constructor: CuVariant::CuVariant(const std::vector<unsigned long> &lui)
 */
std::vector<unsigned long int> CuVariant::toULongIntVector() const  {
    unsigned long int *i_val = static_cast<unsigned long int *>(_d->val);
    std::vector<unsigned long int> ivalues(i_val, i_val + _d->mSize);
    return ivalues;
}

/** \brief convert the stored data into a vector of long integers
 *
 * @return std::vector<long int> representation of the stored data
 *
 * Compatible constructor: CuVariant::CuVariant(const std::vector<long> &li)
 */
std::vector<long int> CuVariant::toLongIntVector() const {
    long int *i_val = (long int *) _d->val;
    std::vector<long int> ivalues(i_val, i_val + _d->mSize);
    return ivalues;
}

std::vector<long long> CuVariant::toLongLongIntVector() const {
    long long int *ll_i_val = (long long int *) _d->val;
    std::vector<long long int> ll_ivalues(ll_i_val, ll_i_val + _d->mSize);
    return ll_ivalues;
}

/** \brief convert the stored data into a vector of booleans
 *
 * @return std::vector<bool> representation of the stored data
 *
 * Compatible constructor: CuVariant::CuVariant(const std::vector<bool> &vb)
 */
std::vector<bool> CuVariant::toBoolVector() const {
    bool *b_val = (bool *) _d->val;
    std::vector<bool> bvalues(b_val, b_val + _d->mSize);
    return bvalues;
}

/** \brief convert the stored data into a vector of short integers
 *
 * @return std::vector<short> representation of the stored data
 *
 * Compatible constructor: CuVariant::CuVariant(const std::vector<short> &si)
 */
std::vector<short> CuVariant::toShortVector() const {
    short *v_s = static_cast<short *>(_d->val);
    std::vector<short> svals(v_s, v_s + _d->mSize);
    return svals;
}

/** \brief convert the stored data into a vector of unsigned short integers
 *
 * @return std::vector<unsigned short> representation of the stored data
 *
 * Compatible constructor: CuVariant::CuVariant(const std::vector<unsigned short> &si)
 */
std::vector<unsigned short> CuVariant::toUShortVector() const {
    unsigned short *v_us = static_cast<unsigned short *>(_d->val);
    std::vector<unsigned short> usvals(v_us, v_us + _d->mSize);
    return usvals;
}

/*!
 * \brief convert the stored data into a vector of char
 * \return std::vector< char> representation of the stored data
 */
std::vector<char> CuVariant::toCharVector() const {
    char *v_uc = static_cast<char *>(_d->val);
    std::vector<char> ucvals(v_uc, v_uc + _d->mSize);
    return ucvals;
}

/*!
 * \brief convert the stored data into a vector of unsigned char
 * \return std::vector<unsigned char> representation of the stored data
 */
std::vector<unsigned char> CuVariant::toUCharVector() const {
    unsigned char *v_uc = static_cast<unsigned char *>(_d->val);
    std::vector<unsigned char> ucvals(v_uc, v_uc + _d->mSize);
    return ucvals;
}

/** \brief convert the stored data into a double scalar
 *
 * @param *ok a pointer to a bool. If not null, its value will be set to true
 *        if the conversion is successful, false otherwise (wrong data type,
 *        format, invalid CuVariant or NULL value)
 * @return the double value held in this object, or *NaN* if either the data type
 *         or format is wrong, the CuVariant is not valid or the value is NULL
 */
double CuVariant::toDouble(bool *ok) const {
    double v = nan("NaN");
    bool can_convert = (_d->type == Double && _d->format == Scalar && _d->val != NULL && _d->mIsValid);
    if(can_convert)
        v = *((double *)_d->val);
    if(ok)
        *ok = can_convert;
    return v;
}

double CuVariant::d() const { return toDouble(nullptr); }

/** \brief convert the stored data into a long double scalar
 *
 * @param *ok a pointer to a bool. If not null, its value will be set to true
 *        if the conversion is successful, false otherwise (wrong data type,
 *        format, invalid CuVariant or NULL value)
 * @return the long double value held in this object, or *NaN* if either the data type
 *         or format is wrong, the CuVariant is not valid or the value is NULL
 */
long double CuVariant::toLongDouble(bool *ok) const {
    long double v = nanl("NaN");
    bool can_convert = (_d->type == LongDouble && _d->format == Scalar && _d->val != NULL && _d->mIsValid);
    if(can_convert)
        v = *((long double *)_d->val);
    if(ok)
        *ok = can_convert;
    return v;
}

long double CuVariant::ld() const { return toLongDouble(nullptr); }

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
long int CuVariant::toLongInt(bool *ok) const {
    long int i = LONG_MIN;
    bool canConvert = (_d->type == Int || _d->type == LongInt)  && _d->format == Scalar && _d->val != NULL && _d->mIsValid;
    if(canConvert && _d->format == Scalar && _d->val != NULL && _d->type == Int)
        i = static_cast<long int> ( *(static_cast<int *>(_d->val)) );
    else if(canConvert && _d->type == LongInt)
        i = *(static_cast<long int*>(_d->val));
    if(ok)
        *ok = canConvert;
    return i;
}

long CuVariant::l() const { return toLongInt(nullptr); }

long long CuVariant::toLongLongInt(bool *ok) const {
#ifdef LONG_LONG_MIN
    long long int i = LONG_LONG_MIN;
#else
    long long int i = LONG_MIN;
#endif
    bool canConvert = (_d->type == Int || _d->type == LongInt || _d->type == LongLongInt)  && _d->format == Scalar && _d->val != NULL && _d->mIsValid;
    if(canConvert && _d->format == Scalar && _d->val != NULL && _d->type == Int)
        i = static_cast<long long int> ( *(static_cast<int *>(_d->val)) );
    else if(canConvert && _d->type == LongInt)
        i = static_cast<long long int> (*(static_cast<long int*>(_d->val)));
    else if(canConvert && _d->type == LongLongInt)
        return *(static_cast<long long int*>(_d->val));
    if(ok)
        *ok = canConvert;
    return i;
}

long long CuVariant::ll() const { return toLongLongInt(); }

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
int CuVariant::toInt(bool *ok) const {
    int i = INT_MIN;
    bool canConvert = (_d->type == Int || _d->type == Short)  && _d->format == Scalar && _d->val != NULL && _d->mIsValid;
    if(canConvert && _d->type == Int)
        i = *(static_cast<int *>(_d->val) );
    else if(canConvert && _d->type == Short)
        i = static_cast<int>( *(static_cast<short int *>(_d->val)));
    if(ok)
        *ok = canConvert;
    return i;
}

int CuVariant::i() const { return toInt(nullptr); }

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
unsigned int CuVariant::toUInt(bool *ok) const {
    unsigned int i = UINT_MAX;
    bool can_convert = (_d->format == Scalar && _d->val != NULL  && _d->mIsValid && (_d->type == UInt || _d->type == UShort) );
    if(can_convert && _d->type == UInt)
        i = *(static_cast<unsigned int *>(_d->val) );
    else if(can_convert && _d->type == UShort)
        i = static_cast<unsigned int>( *(static_cast<unsigned short *>(_d->val)) );
    if(ok)
        *ok = can_convert;
    return i;
}

unsigned int CuVariant::u() const { return toUInt(nullptr); }

unsigned long long int CuVariant::toULongLongInt(bool *ok) const {
#ifdef ULONG_LONG_MAX
    unsigned long long int i = ULONG_LONG_MAX;
#else
    unsigned long long int i = LONG_MAX;
#endif
    bool can_convert = (_d->format == Scalar && _d->val != NULL  && _d->mIsValid &&
            (_d->type == UInt || _d->type == UShort || _d->type == LongLongUInt || _d->type == LongUInt) );
    if(can_convert && _d->type == UInt)
        i = *(static_cast<unsigned int *>(_d->val) );
    else if(can_convert && _d->type == UShort)
        i = static_cast<unsigned long long int>( *(static_cast<unsigned long long int *>(_d->val)) );
    else if(can_convert && _d->type == LongUInt)
        i = static_cast<unsigned long long int>( *(static_cast<unsigned long long int *>(_d->val)) );
    else if(can_convert && _d->type == LongLongUInt)
        i = *(static_cast<unsigned long long *>(_d->val));
    if(ok)
        *ok = can_convert;
    return i;
}

unsigned long long CuVariant::ull() const { return toULongLongInt(nullptr); }



/** \brief convert the stored data into a scalar unsigned short
 *
 * @param *ok a pointer to a bool. If not null, its value will be set to true
 *        if the conversion is successful, false otherwise (wrong data type,
 *        format, invalid CuVariant or NULL value)
 * @return the unsigned short value held in this object, or USHRT_MAX if either the data type
 *         or format is wrong, the CuVariant is not valid or the value is NULL
 *
 */
unsigned short CuVariant::toUShortInt(bool *ok) const {
    unsigned short s = USHRT_MAX;
    bool canConvert = _d->type == UShort && _d->format == Scalar && _d->val != NULL && _d->mIsValid;
    if(canConvert )
        s = *(static_cast<unsigned short *>(_d->val));
    if(ok)
        *ok = canConvert;
    return s;
}

unsigned short CuVariant::us() const { return toUShortInt(nullptr); }

/** \brief convert the stored data into a scalar signed short
 *
 * @param *ok a pointer to a bool. If not null, its value will be set to true
 *        if the conversion is successful, false otherwise (wrong data type,
 *        format, invalid CuVariant or NULL value)
 * @return the signed short value held in this object, or SHRT_MIN if either the data type
 *         or format is wrong, the CuVariant is not valid or the value is NULL
 *
 */
short CuVariant::toShortInt(bool *ok) const {
    short s = SHRT_MIN;
    bool canConvert = _d->type == Short && _d->format == Scalar && _d->val != NULL && _d->mIsValid;
    if(canConvert)
        s = *(static_cast<short *>(_d->val));
    if(ok)
        *ok = canConvert;
    return s;
}

short CuVariant::si() const { return toShortInt(nullptr); }

/** \brief convert the stored data into a scalar floating point number
 *
 * @param *ok a pointer to a bool. If not null, its value will be set to true
 *        if the conversion is successful, false otherwise (wrong data type,
 *        format, invalid CuVariant or NULL value)
 * @return the float value held in this object, or NaN (as returned by nanf) if either the data type
 *         or format is wrong, the CuVariant is not valid or the value is NULL
 *
 */
float CuVariant::toFloat(bool *ok) const {
    float f = nanf("NaN");
    bool canConvert = _d->type == Float && _d->format == Scalar && _d->val != NULL && _d->mIsValid;
    if(canConvert)
        f = *(static_cast<float *>(_d->val));
    if(ok)
        *ok = canConvert;
    return f;
}

float CuVariant::f() const { return toFloat(nullptr); }

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
unsigned long int CuVariant::toULongInt(bool *ok) const {
    unsigned long int i = ULONG_MAX;
    bool canConvert = (_d->type == UInt || _d->type == LongUInt)  && _d->format == Scalar && _d->val != NULL && _d->mIsValid;
    if(canConvert && _d->type == UInt)
        i = static_cast<unsigned long int> ( *(static_cast<unsigned int* >(_d->val) ) );
    else if(canConvert && _d->type == LongUInt)
        i = *(static_cast<unsigned  long int* >(_d->val) );
    if(ok)
        *ok = canConvert;
    return i;
}

unsigned long CuVariant::ul() const { return toULongInt(nullptr); }


/** \brief convert the stored data into a scalar boolean
 *
 * @param *ok a pointer to a bool. If not null, its value will be set to true
 *        if the conversion is successful, false otherwise (wrong data type,
 *        format, invalid CuVariant or NULL value)
 * @return the bool value held in this object, or ULONG_MAX if either the data type
 *         or format is wrong, the CuVariant is not valid or the value is NULL
 */
bool CuVariant::toBool(bool *ok) const {
    bool b = false;
    bool can_convert = (_d->type == Boolean && _d->format == Scalar && _d->val != NULL && _d->mIsValid);
    if(can_convert)
        b = *(static_cast<bool *> (_d->val) );
    if(ok)
        *ok = can_convert;
    return b;
}

bool CuVariant::b() const { return toBool(nullptr); }

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
 * \par Note is format is empty, a default conversion takes place according to the type of data, e.g.
 *      "%f" for double and float, "%Lf" for long double, "%d" for int, "%lu" for long unsigned int,
 *      "%ld" for long int, "%hd" for short int, "%hu" for unsigned short.
 *
 * \par Boolean data types
 * boolean values are mapped to either "true" or "false" string.
 *
 */
std::string CuVariant::toString(bool *ok, const char *format) const {
    const size_t MAXLEN = 128;
    std::string ret;
    char converted[MAXLEN + 1];
    bool success = _d->mSize > 0;
    memset(converted, 0, sizeof(char) * (MAXLEN + 1));
    for(size_t i = 0; (_d->format == Scalar || _d->format == Vector) &&  i < _d->mSize && success; i++)
    {
        if(i > 0)
            ret += ",";
        if(_d->type == String)
            ret += std::string(static_cast<char **>(_d->val)[i]);
        else if(_d->type == Double)
            snprintf(converted, MAXLEN, strlen(format) > 0 ? format : "%g", static_cast<double *>(_d->val)[i]);
        else if(_d->type == LongDouble)
            snprintf(converted, MAXLEN, strlen(format) > 0 ? format : "%Lf", static_cast<long double *>(_d->val)[i]);
        else if(_d->type == Int)
            snprintf(converted, MAXLEN, strlen(format) > 0 ? format : "%d", static_cast<int *>(_d->val)[i]);
        else if(_d->type == UInt)
            snprintf(converted, MAXLEN, strlen(format) > 0 ? format : "%u", static_cast<unsigned int *>(_d->val)[i]);
        else if(_d->type == LongUInt)
            snprintf(converted, MAXLEN, strlen(format) > 0 ? format : "%lu", static_cast<long unsigned int *>(_d->val)[i]);
        else if(_d->type == LongLongUInt)
            snprintf(converted, MAXLEN, strlen(format) > 0 ? format : "%llu", static_cast<long long unsigned int *>(_d->val)[i]);
        
        else if(_d->type == LongInt)
            snprintf(converted, MAXLEN, strlen(format) > 0 ? format : "%ld", static_cast<long int *>(_d->val)[i]);
        else if(_d->type == LongLongInt)
            snprintf(converted, MAXLEN, strlen(format) > 0 ? format : "%lld", static_cast<long long int *>(_d->val)[i]);
        else if(_d->type == Short)
            snprintf(converted, MAXLEN, strlen(format) > 0 ? format : "%hd", static_cast<short int *>(_d->val)[i]);
        else if(_d->type == UShort)
            snprintf(converted, MAXLEN, strlen(format) > 0 ? format : "%hu", static_cast<unsigned short *>(_d->val)[i]);
        else if(_d->type == Char)
            snprintf(converted, MAXLEN, strlen(format) > 0 ? format : "%d", static_cast<char *>(_d->val)[i]);
        else if(_d->type == UChar)
            snprintf(converted, MAXLEN, strlen(format) > 0 ? format : "%d", static_cast<unsigned char *>(_d->val)[i]);
        else if(_d->type == Float)
            snprintf(converted, MAXLEN, strlen(format) > 0 ? format : "%g", static_cast<float *>(_d->val)[i]);
        else if(_d->type == Boolean)
            static_cast<bool *>(_d->val)[i] ? sprintf(converted, "true") : sprintf(converted, "false");
        else if(_d->type == VoidPtr)
            snprintf(converted, MAXLEN, "%p", _d->val);
        else
        {
            success = false;
            ret = "";
            perr("CuVariant.toString: error converting data to string: format is %d type is %d",
                 _d->format, _d->type);
        }
        
        if(_d->type != String)
            ret += std::string(converted);
    }
    if(_d->format == Matrix) {
        switch(_d->type) {
        case UChar: {
            CuMatrix<unsigned char> *m = static_cast<CuMatrix <unsigned char > * >(_d->val);
            snprintf(converted, MAXLEN, "%s", m->repr().c_str() );
        }break;
        case Char: {
            CuMatrix<char> *m = static_cast<CuMatrix <char > * >(_d->val);
            snprintf(converted, MAXLEN, "%s", m->repr().c_str() );
            break;
        }
        case UShort: {
            CuMatrix<unsigned short> *m = static_cast<CuMatrix <unsigned short > * >(_d->val);
            snprintf(converted, MAXLEN, "%s", m->repr().c_str() );
        }break;
        case Short: {
            CuMatrix<short> *m = static_cast<CuMatrix <short > * >(_d->val);
            snprintf(converted, MAXLEN, "%s", m->repr().c_str() );
        } break;
        case Float: {
            CuMatrix<float> *m = static_cast<CuMatrix <float > * >(_d->val);
            snprintf(converted, MAXLEN, "%s", m->repr().c_str() );
        } break;
        case Double: {
            CuMatrix<double> *m = static_cast<CuMatrix <double > * >(_d->val);
            snprintf(converted, MAXLEN, "%s", m->repr().c_str() );
        } break;
        case LongDouble: {
            CuMatrix<long double> *m = static_cast<CuMatrix <long double > * >(_d->val);
            snprintf(converted, MAXLEN, "%s", m->repr().c_str() );
        } break;
        case Int: {
            CuMatrix<int> *m = static_cast<CuMatrix <int > * >(_d->val);
            snprintf(converted, MAXLEN, "%s", m->repr().c_str() );
        } break;
        case LongInt: {
            CuMatrix<long int> *m = static_cast<CuMatrix <long int > * >(_d->val);
            snprintf(converted, MAXLEN, "%s", m->repr().c_str() );
        } break;
        case LongLongInt: {
            CuMatrix<long long int> *m = static_cast<CuMatrix <long long int > * >(_d->val);
            snprintf(converted, MAXLEN, "%s", m->repr().c_str() );
        } break;
        case UInt: {
            CuMatrix<unsigned int> *m = static_cast<CuMatrix <unsigned int > * >(_d->val);
            snprintf(converted, MAXLEN, "%s", m->repr().c_str() );
        } break;
        case LongUInt: {
            CuMatrix<unsigned long int> *m = static_cast<CuMatrix <unsigned long int > * >(_d->val);
            snprintf(converted, MAXLEN, "%s", m->repr().c_str() );
        } break;
        case LongLongUInt: {
            CuMatrix<unsigned long long int> *m = static_cast<CuMatrix <unsigned long long int > * >(_d->val);
            snprintf(converted, MAXLEN, "%s", m->repr().c_str() );
        } break;
        case Boolean: {
            CuMatrix<bool> *m = static_cast<CuMatrix <bool> * >(_d->val);
            snprintf(converted, MAXLEN, "%s", m->repr().c_str() );
        }break;
        case String: {
            cuprintf("CuVariant.toString: Matrix, std::string\n");
            CuMatrix<std::string> *m = static_cast<CuMatrix <std::string> * >(_d->val);
            snprintf(converted, MAXLEN, "%s", m->repr().c_str() );
        }break;
        default:
            perr("CuVariant.toString: error converting matrix data to string: type is %d (%s)",
                 _d->type, dataTypeStr(_d->type).c_str());
            break;
        }
        ret = std::string(converted);
        
    }// format matrix
    
    if(ok)
        *ok = success;
    return ret;
}

std::string CuVariant::s() const { return toString(nullptr); }

std::string CuVariant::s(const char *fmt, bool *ok) const { return toString(ok, fmt); }

std::vector<std::string> CuVariant::toStringVector(bool *ok) const {
    return toStringVector(nullptr, ok);
}

std::vector<std::string> CuVariant::toStringVector(const char *fmt) const {
    return toStringVector(fmt, nullptr);
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
std::vector<std::string> CuVariant::toStringVector(const char *fmt, bool *ok) const {
    bool success = true;
    bool native_type = (_d->type == String && (_d->format == Vector || _d->format == Scalar) );
    std::vector<std::string> ret;
    if(native_type)
    {
        char **str_array = static_cast<char **>(_d->val);
        for(size_t i = 0; i < _d->mSize; i++)
            ret.push_back(std::string(str_array[i]));
    }
    else if(_d->format == Vector || _d->format == Scalar) {
        const size_t MAXLEN = 128;
        char converted[MAXLEN + 1];
        
        for(size_t i = 0; i < _d->mSize && success; i++) // while success is true
        {
            memset(converted, 0, sizeof(char) * (MAXLEN + 1));  // clear string
            
            if(_d->type == String) // directly push back the native data
                ret.push_back(std::string(static_cast<char **>(_d->val)[i]));
            else if(_d->type == Double)
                snprintf(converted, MAXLEN, !fmt ? "%f" : fmt, static_cast<double *>(_d->val)[i]);
            else if(_d->type == LongDouble)
                snprintf(converted, MAXLEN,  !fmt ? "%Lf" : fmt, static_cast<long double *>(_d->val)[i]);
            else if(_d->type == Int)
                snprintf(converted, MAXLEN, !fmt ? "%d" : fmt, static_cast<int *>(_d->val)[i]);
            else if(_d->type == UInt)
                snprintf(converted, MAXLEN, !fmt ? "%u" : fmt, static_cast<unsigned int *>(_d->val)[i]);
            else if(_d->type == LongUInt)
                snprintf(converted, MAXLEN, !fmt ? "%lu" : fmt, static_cast<long unsigned int *>(_d->val)[i]);
            else if(_d->type == LongInt)
                snprintf(converted, MAXLEN, !fmt ? "%ld" : fmt, static_cast<long int *>(_d->val)[i]);
            else if(_d->type == Short)
                snprintf(converted, MAXLEN, !fmt ? "%hd" : fmt, static_cast<short int *>(_d->val)[i]);
            else if(_d->type == UShort)
                snprintf(converted, MAXLEN, !fmt ? "%hu" : fmt, static_cast<unsigned short *>(_d->val)[i]);
            else if(_d->type == Float)
                snprintf(converted, MAXLEN, !fmt ? "%f" : fmt, static_cast<float *>(_d->val)[i]);
            else if(_d->type == Boolean)
                static_cast<bool *>(_d->val)[i] ? sprintf(converted, "true") : sprintf(converted, "false");
            else if(_d->type == UChar)
                snprintf(converted, MAXLEN, !fmt ? "%u" : fmt, static_cast<unsigned char *>(_d->val)[i]);
            else if(_d->type == Char)
                snprintf(converted, MAXLEN, !fmt ? "%d" : fmt, static_cast<char *>(_d->val)[i]);
            else {
                success = false;
                perr("CuVariant.toStringVector: error converting data to string vector: format is %s type is %s",
                     dataFormatStr(_d->format).c_str(), dataTypeStr(_d->type).c_str());
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
    if(!success && !_d->mIsNull) {
        perr("CuVariant::toStringVector: cannot convert type %s format %s to string vector [%s] isNull %d isValid %d",
             dataTypeStr(_d->type).c_str(), dataFormatStr(_d->format).c_str(), toString().c_str(), _d->mIsNull, _d->mIsValid);
    }
    return ret;
}

std::vector<std::string> CuVariant::sv() const { return toStringVector(nullptr, nullptr); }

std::vector<std::string> CuVariant::sv(const char *fmt, bool *ok) const {
    return toStringVector(fmt, ok);
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
double *CuVariant::toDoubleP() const {
    if(_d->type == CuVariant::Double)
        return static_cast<double *> (_d->val);
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
long double *CuVariant::toLongDoubleP() const {
    if(_d->type == CuVariant::LongDouble)
        return static_cast<long double *> (_d->val);
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
unsigned int *CuVariant::toUIntP() const {
    if(_d->type == CuVariant::UInt) return static_cast<unsigned int *>( _d->val );
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
int *CuVariant::toIntP() const {
    if(_d->type == CuVariant::Int) return static_cast<int *>( _d->val );
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
long int *CuVariant::toLongIntP() const {
    if(_d->type == CuVariant::LongInt)
        return static_cast<long int *>(_d->val);
    return NULL;
}

long long int *CuVariant::toLongLongIntP() const {
    if(_d->type == CuVariant::LongLongInt)
        return static_cast<long long int *>(_d->val);
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
unsigned long int *CuVariant::toULongIntP() const {
    if(_d->type == CuVariant::LongUInt)
        return static_cast<unsigned long int *>(_d->val);
    return NULL;
}

unsigned long long *CuVariant::toULongLongIntP() const {
    if(_d->type == CuVariant::LongLongUInt)
        return static_cast<unsigned long long int *>(_d->val);
    return nullptr;
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
float *CuVariant::toFloatP() const {
    if(_d->type == CuVariant::Float)
        return static_cast<float *>(_d->val);
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
short *CuVariant::toShortP() const {
    if(_d->type == CuVariant::Short)
        return static_cast<short int *>(_d->val);
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
unsigned short *CuVariant::toUShortP() const {
    if(_d->type == CuVariant::UShort)
        return static_cast<unsigned short *>(_d->val);
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
bool *CuVariant::toBoolP() const {
    if(_d->type == CuVariant::Boolean)
        return static_cast<bool *> (_d->val );
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
char **CuVariant::to_C_charP() const {
    if(_d->type == CuVariant::String)
        return (char **) _d->val ;
    return NULL;
}


char *CuVariant::toCharP() const {
    if(_d->type == CuVariant::Char)
        return (char *) _d->val ;
    return nullptr;
}

unsigned char *CuVariant::toUCharP() const {
    if(_d->type == CuVariant::UChar)
        return (unsigned char *) _d->val ;
    return nullptr;
}

void *CuVariant::toVoidP() const {
    if(_d->type == CuVariant::VoidPtr) {
        return _d->val;
    }
    return NULL;
}

/*!
 * \brief Append the content of another variant to this variant
 *
 * \param other another CuVariant
 *
 * The data stored into this variant is converted to a std::vector of the
 * same type using the native *toXXXVector* method.
 * The *other* variant value is converted to a std::vector of
 * the type of *this* variant using CuVariant::toVector<T>.
 *
 * The vector from the *other* variant is then appended to the vector from *this*
 * variant.
 *
 * \note
 * CuVariant::append for Matrix format is not supported
 */
void CuVariant::append(const CuVariant &other) {
    m_detach();
    // cast to DataType so that we get warned by the compiler if
    // a case is not handled
    if(other.getFormat() == Matrix || _d->format == Matrix) {
        perr("CuVariant.append: cannot append data in Matrix format");
    }
    else {
        DataType dt = static_cast<DataType> (_d->type);
        switch (dt) {
        case Short: {
            std::vector<short> t_sv = toShortVector(), o_sv;
            other.toVector<short>(o_sv);
            t_sv.insert(t_sv.end(), o_sv.begin(), o_sv.end());
            *this = CuVariant(t_sv);
        }
            break;
        case UShort: {
            std::vector<unsigned short> t_usv = toUShortVector(), o_usv;
            other.toVector<unsigned short>(o_usv);
            t_usv.insert(t_usv.end(), o_usv.begin(), o_usv.end());
            *this = CuVariant(t_usv);
        }
            break;
        case Int: {
            std::vector<int> t_iv = toIntVector(), o_iv;
            other.toVector<int>(o_iv);
            t_iv.insert(t_iv.end(), o_iv.begin(), o_iv.end());
            *this = CuVariant(t_iv);
        }
            break;
        case UInt: {
            std::vector<unsigned int> t_uiv = toUIntVector(), o_uiv;
            other.toVector<unsigned int>(o_uiv);
            t_uiv.insert(t_uiv.end(), o_uiv.begin(), o_uiv.end());
            *this = CuVariant(t_uiv);
        }
            break;
        case LongInt: {
            std::vector<long int> t_liv = toLongIntVector(), o_liv;
            other.toVector<long int>(o_liv);
            t_liv.insert(t_liv.end(), o_liv.begin(), o_liv.end());
            *this = CuVariant(t_liv);
        }
            break;
        case LongUInt: {
            std::vector<unsigned long int> t_uliv = this->toULongIntVector(), o_uliv;
            other.toVector<unsigned long int>(o_uliv);
            t_uliv.insert(t_uliv.end(), o_uliv.begin(), o_uliv.end());
            *this = CuVariant(t_uliv);
        }
            break;
        case Float: {
            std::vector<float> t_fv = this->toFloatVector(), o_fv;
            other.toVector<float>(o_fv);
            t_fv.insert(t_fv.end(), o_fv.begin(), o_fv.end());
            *this = CuVariant(t_fv);
        }
            break;
        case Double: {
            std::vector<double> t_dv = this->toDoubleVector(), o_dv;
            other.toVector<double>(o_dv);
            t_dv.insert(t_dv.end(), o_dv.begin(), o_dv.end());
            *this = CuVariant(t_dv);
        }
            break;
        case LongDouble: {
            std::vector<long double> t_ldv = this->toLongDoubleVector(), o_ldv;
            other.toVector<long double>(o_ldv);
            t_ldv.insert(t_ldv.end(), o_ldv.begin(), o_ldv.end());
            *this = CuVariant(t_ldv);
        }
            break;
        case Boolean: {
            std::vector<bool> t_bv = this->toBoolVector(), o_bv;
            other.toVector<bool>(o_bv);
            t_bv.insert(t_bv.end(), o_bv.begin(), o_bv.end());
            *this = CuVariant(t_bv);
        }
            break;
        case String: {
            std::vector<std::string> t_sv = this->toStringVector(), o_sv;
            o_sv = other.toStringVector();
            t_sv.insert(t_sv.end(), o_sv.begin(), o_sv.end());
            *this = CuVariant(t_sv);
        }
            break;
        case LongLongInt: {
            std::vector<long long int> t_lliv = toLongLongIntVector(), o_lliv;
            other.toVector<long long int>(o_lliv);
            t_lliv.insert(t_lliv.end(), o_lliv.begin(), o_lliv.end());
            *this = CuVariant(t_lliv);
        }
        case LongLongUInt: {
            std::vector<long long unsigned int> t_lluiv = toLongLongUIntVector(), o_lluiv;
            other.toVector<long long  unsigned int>(o_lluiv);
            t_lluiv.insert(t_lluiv.end(), o_lluiv.begin(), o_lluiv.end());
            *this = CuVariant(t_lluiv);
        }
            break;
        case VoidPtr: {
            perr("CuVariant.append: cannot append data to a VoidPtr CuVariant");
        }
            break;
        case UChar: {
            std::vector<unsigned char> t_uchv = toUCharVector(), o_uchv;
            other.toVector<unsigned char>(o_uchv);
            t_uchv.insert(o_uchv.end(), o_uchv.begin(), o_uchv.end());
            *this = CuVariant(o_uchv);
        }
        case Char: {
            std::vector<char> t_chv = toCharVector(), o_chv;
            other.toVector<char>(o_chv);
            t_chv.insert(o_chv.end(), o_chv.begin(), o_chv.end());
            *this = CuVariant(o_chv);
        }
        case TypeInvalid:
            if(other._d->type != TypeInvalid) {
                *this = CuVariant(other);
                this->_d->format = Vector;
            }
            
            else
                perr("CuVariant.append: cannot cat two invalid CuVariants");
            break;
        case EndDataTypes:
            break;
        }
    }
}

/** \brief Change the storage format to Vector
 *
 * modify the CuVariant::DataFormat to vector, so that data can
 * be interpreted as vector
 */
CuVariant& CuVariant::toVector() {
    m_detach();
    _d->format = Vector;
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
        "LongInt", "LongLongInt", "LongUInt", "LongLongUInt", "Float", "Double",
        "LongDouble", "Boolean", "String", "VoidPtr", "Char", "UChar", "EndDataTypes"
    };
    if(t >= 0 && t < EndDataTypes)
        return std::string(v[t]);
    return std::string(" OutOfRange");
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
    return std::string(" OutOfRange");
}

