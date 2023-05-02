#ifndef CUVARIANT_T_H
#define CUVARIANT_T_H

#include "cuvariantprivate.h"
#include <cuvariant.h>
#include <string> // stold
#include <stdexcept>
#include <stdlib.h>

template<typename T>
CuVariant::CuVariant(const T *p, size_t siz, DataFormat f, DataType t, int rows) {
    _d = new CuVariantPrivate(); // _d->val is nullptr
    m_init(f, t); // sets _d->mIsNull and !_d->mIsValid
    _d->mIsNull = (p == nullptr || siz == 0);
    _d->mIsValid = !_d->mIsNull;
    if(_d->mIsValid && f == DataFormat::Matrix && rows > 0) {
        _d->mSize = siz;
        _d->nrows = rows;
        _d->ncols = siz / rows;
        _d->val = new CuMatrix<T>(p, rows, siz / rows);
    }
    else if(_d->mIsValid) {
        _d->mSize = _d->nrows = siz;
        _d->val = new T[siz];
        memcpy(_d->val, p, siz * sizeof(T));
    }
}

/** \brief convert the current *scalar* CuVariant into a variable of type T.
 *
 * @param val a reference to a variable of type T
 * @return true the conversion is successful
 * @return false the conversion was not possible
 *
 * This method tries to convert the value stored into the CuVariant into the desired
 * type passed as reference.
 *
 * \par Warning
 * If the CuVariant is null or the format is not scalar or the variant is in general invalid,
 * *val* will contain the 0ULL value casted to T
 *
 * @see toVector
 */
template<typename T> bool CuVariant::to(T &val) const
{
    bool valid = ((_d->format == Scalar || (_d->format == Vector && _d->mSize > 0)) && !_d->mIsNull && _d->mIsValid);
    if(!valid) {
        val = static_cast<T> (0ULL);
    }
    else if(_d->format == Scalar || _d->format == Vector) {
        switch(_d->type)
        {
        case Short:
            val = static_cast<T>(static_cast<short *>(_d->val)[0]);
            break;
        case UShort:
            val = static_cast<T>(static_cast<unsigned short *>(_d->val)[0]);
            break;
        case Int:
            val = static_cast<T>(static_cast<int *>(_d->val)[0]);
            break;
        case UInt:
            val = static_cast<T>(static_cast<unsigned int *>(_d->val)[0]);
            break;
        case Char:
            val = static_cast<T>(static_cast<char *>(_d->val)[0]);
            break;
        case UChar:
            val = static_cast<T>(static_cast<unsigned char *>(_d->val)[0]);
            break;
        case LongInt:
            val = static_cast<T>(static_cast<long int *>(_d->val)[0]);
            break;
        case LongLongInt:
            val = static_cast<T>(static_cast<long long int *>(_d->val)[0]);
            break;
        case LongUInt:
            val = static_cast<T>(static_cast<unsigned long *>(_d->val)[0]);
            break;
        case LongLongUInt:
            val = static_cast<T>(static_cast<unsigned long long*>(_d->val)[0]);
            break;
        case Float:
            val = static_cast<T>(static_cast<float *>(_d->val)[0]);
            break;
        case Double:
            val = static_cast<T>(static_cast<double *>(_d->val)[0]);
            break;
        case LongDouble:
            val = static_cast<T>(static_cast<long double *>(_d->val)[0]);
            break;
        case Boolean:
            val = static_cast<T>(static_cast<bool *>(_d->val)[0]);
            break;
        case String: {
            const std::string& s = toString();
            if(s == "true") val = static_cast<T>(1);
            else if(s == "false") val = static_cast<T>(0);
            else { // try converting to long double
                try {
                    val = static_cast<T>( std::stold(s));
                }
                catch(const std::invalid_argument& ) {
                    pwarn("CuVariant.to: string \"%s\" to number conversion failed: invalid argument", toString().c_str());
                    valid = false;
                }
                catch(const std::out_of_range& ) {
                    pwarn("CuVariant.to: string \"%s\" to number conversion failed: out of range", toString().c_str());
                    valid = false;
                }
            }
        }
            break;
        default:
            valid = false;
            break;
        }
        if(!valid)
            pwarn("%s CuVariant.to: unsupported scalar conversion from type %s and format %s",
                 __PRETTY_FUNCTION__, dataTypeStr(_d->type).c_str(), dataFormatStr(_d->format).c_str());
    }
    return valid;
}

/** \brief convert the current CuVariant into a vector of type T, if possible.
 *
 * @param val a reference to a std vector of type T
 * @return true the conversion is successful
 * @return false the conversion was not possible
 *
 * This method tries to convert the value stored into the CuVariant into the desired
 * vector of type T passed as reference.
 *
 * @see to
 *
 * \note if the CuVariant::DataFormat is CuVariant::Scalar, v will contain only one
 *       element with the scalar value
 */
template<typename T> bool CuVariant::toVector(std::vector<T> &v) const
{
    bool valid = true;
    size_t i;
    if(_d->format == Vector)
    {
        if(_d->type == Short)
        {
            std::vector<short> vs = toShortVector();
            for(i = 0; i < vs.size(); i++)
                v.push_back(static_cast<T>(vs[i]));
        }
        else if(_d->type == UShort) {
            std::vector<unsigned short> vus = toUShortVector();
            for(i = 0; i < vus.size(); i++)
                v.push_back(static_cast<T>(vus[i]));
        }
        else if(_d->type == UChar) {
            std::vector<unsigned char> vuc = toUCharVector();
            for(i = 0; i < vuc.size(); i++)
                v.push_back(static_cast<T>(vuc[i]));
        }
        else if(_d->type == Char) {
            std::vector<char> vc = toCharVector();
            for(i = 0; i < vc.size(); i++)
                v.push_back(static_cast<T>(vc[i]));
        }
        else if(_d->type == Int) {
            std::vector<int> vi = toIntVector();
            for(i = 0; i < vi.size(); i++)
                v.push_back(static_cast<T>(vi[i]));
        }
        else if(_d->type == UInt) {
            std::vector<unsigned int> vui = toUIntVector();
            for(i = 0; i < vui.size(); i++)
                v.push_back(static_cast<T>(vui[i]));
        }
        else if(_d->type == LongInt) {
            std::vector<long int> vli = toLongIntVector();
            for(i = 0; i < vli.size(); i++)
                v.push_back(static_cast<T>(vli[i]));
        }
        else if(_d->type == LongLongInt) {
            std::vector<long long int> vlli = toLongLongIntVector();
            for(i = 0; i < vlli.size(); i++)
                v.push_back(static_cast<T>(vlli[i]));
        }
        else if(_d->type == LongLongUInt) {
            std::vector<unsigned long long int> vlli = toLongLongUIntVector();
            for(i = 0; i < vlli.size(); i++)
                v.push_back(static_cast<T>(vlli[i]));
        }
        else if(_d->type == LongUInt) {
            std::vector<long unsigned int> vlui = toULongIntVector();
            for(i = 0; i < vlui.size(); i++)
                v.push_back(static_cast<T>(vlui[i]));
        }
        else if(_d->type == Float) {
            std::vector<float> vf = toFloatVector();
            for(i = 0; i < vf.size(); i++)
                v.push_back(static_cast<T>(vf[i]));
        }
        else if(_d->type == Double) {
            std::vector<double> df = toDoubleVector();
            for(i = 0; i < df.size(); i++)
                v.push_back(static_cast<T>(df[i]));
        }
        else if(_d->type == LongDouble) {
            std::vector<long double> ldv = toLongDoubleVector();
            for(i = 0; i < ldv.size(); i++)
                v.push_back(static_cast<T>(ldv[i]));
        }
        else if(_d->type == Boolean) {
            std::vector<bool> bv = toBoolVector();
            for(i = 0; i < bv.size(); i++)
                v.push_back(static_cast<T>(bv[i]));
        }
        else if(_d->type == String) {
            long double ld;
            std::vector<std::string> sv = toStringVector();
            for(i = 0; i < sv.size() && valid; i++)
            {
                try
                {
                    // try converting to long double
                    ld = std::stold(sv[i]);
                    v.push_back(static_cast<T>(ld));
                }
                catch(const std::invalid_argument& ) {
                    pwarn("CuVariant.toVector: string \"%s\" to number conversion failed: invalid argument", toString().c_str());
                    valid = false;
                }
                catch(const std::out_of_range &) {
                    pwarn("CuVariant.toVector: string \"%s\" to number conversion failed: out of range", toString().c_str());
                    valid = false;
                }
            }
        }
        else
            valid = false;
    }
    else if(_d->format == Scalar)
    {
        if(_d->type == Short)
            v.push_back(static_cast<T>(toShortInt()));
        else if(_d->type == UShort)
            v.push_back(static_cast<T>(toUShortInt()));
        else if(_d->type == Int)
            v.push_back(static_cast<T>(toInt()));
        else if(_d->type == UInt)
            v.push_back(static_cast<T>(toUInt()));
        else if(_d->type == LongInt)
            v.push_back(static_cast<T>(toLongInt()));
        else if(_d->type == LongUInt)
            v.push_back(static_cast<T>(toULongInt()));
        else if(_d->type == Float)
            v.push_back(static_cast<T>(toFloat()));
        else if(_d->type == Double)
            v.push_back(static_cast<T>(toDouble()));
        else if(_d->type == LongDouble)
            v.push_back(static_cast<T>(toLongDouble()));
        else if(_d->type == Boolean)
            v.push_back(static_cast<T>(toBool()));
        else if(_d->type == String)
        {
            try
            {
                // try converting to long double
                long double ld = std::stold(toString());
                v.push_back(static_cast<T>(ld));
            }
            catch(const std::invalid_argument &) {
                pwarn("CuVariant.toVector: string \"%s\" to number conversion failed: invalid argument", toString().c_str());
                valid = false;
            }
            catch(const std::out_of_range &) {
                pwarn("CuVariant.toVector: string \"%s\" to number conversion failed: out of range", toString().c_str());
                valid = false;
            }
        }
        else
            valid = false;
    }
    return valid;
}


template<typename T>
CuMatrix<T> CuVariant::toMatrix() const {
    if(_d->format == Matrix) {
        return *(static_cast<CuMatrix <T> * >(_d->val));
    }
    return CuMatrix<T>();
}

template<typename T>
CuMatrix<T> *CuVariant::matrix_ptr() const {
    return static_cast<CuMatrix <T> * > (_d->val);
}

#endif // CUVARIANT_T_H
