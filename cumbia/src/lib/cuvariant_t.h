#ifndef CUVARIANT_T_H
#define CUVARIANT_T_H

#include "cuvariantprivate.h"
#include <cuvariant.h>
#include <string> // stold
#include <stdexcept>

/** \brief convert the current *scalar* CuVariant into a variable of type T.
 *
 * @param val a reference to a variable of type T
 * @return true the conversion is successful
 * @return false the conversion was not possible
 *
 * This method tries to convert the value stored into the CuVariant into the desired
 * type passed as reference.
 *
 * @see toVector
 */
template<typename T> bool CuVariant::to(T &val) const
{
    bool valid = (d->format == Scalar);
    if(valid)
    {
        switch(d->type)
        {
        case Short:
            val = static_cast<T>(*(static_cast<short *>(d->val)));
            break;
        case UShort:
            val = static_cast<T>(*(static_cast<unsigned short *>(d->val)));
            break;
        case Int:
            val = static_cast<T>(*(static_cast<int *>(d->val)));
            break;
        case UInt:
            val = static_cast<T>(*(static_cast<unsigned int *>(d->val)));
            break;

        case LongInt:
            val = static_cast<T>(*(static_cast<long int *>(d->val)));
            break;
        case LongUInt:
            val = static_cast<T>(*(static_cast<unsigned long *>(d->val)));
            break;
        case Float:
            val = static_cast<T>(*(static_cast<float *>(d->val)));
            break;
        case Double:
            val = static_cast<T>(*(static_cast<double *>(d->val)));
            break;
        case LongDouble:
            val = static_cast<T>(*(static_cast<long double *>(d->val)));
            break;
        case Boolean:
            val = static_cast<T>(*(static_cast<bool *>(d->val)));
            break;
        case String:
            try
            {
                // try converting to long double
                val = std::stold(this->toString());
        }

            catch(const std::invalid_argument& ) {
                perr("CuVariant.toVector: string \"%s\" to number conversion failed: invalid argument", toString().c_str());
                valid = false;
            }
            catch(const std::out_of_range& ) {
                perr("CuVariant.toVector: string \"%s\" to number conversion failed: out of range", toString().c_str());
                valid = false;
            }
            break;
        default:
            valid = false;
            break;
        }
        if(!valid)
            perr("CuVariant.to: unsupported scalar conversion from type %s and format %s", dataTypeStr(d->type).c_str(),
                 dataFormatStr(d->format).c_str());
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
    if(d->format == Vector)
    {
        if(d->type == Short)
        {
            std::vector<short> vs = toShortVector();
            for(i = 0; i < vs.size(); i++)
                v.push_back(static_cast<T>(vs[i]));
        }
        else if(d->type == UShort) {
            std::vector<unsigned short> vus = toUShortVector();
            for(i = 0; i < vus.size(); i++)
                v.push_back(static_cast<T>(vus[i]));
        }
        else if(d->type == Int) {
            std::vector<int> vi = toIntVector();
            for(i = 0; i < vi.size(); i++)
                v.push_back(static_cast<T>(vi[i]));
        }
        else if(d->type == UInt) {
            std::vector<unsigned int> vui = toUIntVector();
            for(i = 0; i < vui.size(); i++)
                v.push_back(static_cast<T>(vui[i]));
        }
        else if(d->type == LongInt) {
            std::vector<long int> vli = toLongIntVector();
            for(i = 0; i < vli.size(); i++)
                v.push_back(static_cast<T>(vli[i]));
        }
        else if(d->type == LongUInt) {
            std::vector<long unsigned int> vlui = toULongIntVector();
            for(i = 0; i < vlui.size(); i++)
                v.push_back(static_cast<T>(vlui[i]));
        }
        else if(d->type == Float) {
            std::vector<float> vf = toFloatVector();
            for(i = 0; i < vf.size(); i++)
                v.push_back(static_cast<T>(vf[i]));
        }
        else if(d->type == Double) {
            std::vector<double> df = toDoubleVector();
            for(i = 0; i < df.size(); i++)
                v.push_back(static_cast<T>(df[i]));
        }
        else if(d->type == LongDouble) {
            std::vector<long double> ldv = toLongDoubleVector();
            for(i = 0; i < ldv.size(); i++)
                v.push_back(static_cast<T>(ldv[i]));
        }
        else if(d->type == Boolean) {
            std::vector<bool> bv = toBoolVector();
            for(i = 0; i < bv.size(); i++)
                v.push_back(static_cast<T>(bv[i]));
        }
        else if(d->type == String) {
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
                    perr("CuVariant.toVector: string \"%s\" to number conversion failed: invalid argument", toString().c_str());
                    valid = false;
                }
                catch(const std::out_of_range &) {
                    perr("CuVariant.toVector: string \"%s\" to number conversion failed: out of range", toString().c_str());
                    valid = false;
                }
            }
        }
        else
            valid = false;
    }
    else if(d->format == Scalar)
    {
        if(d->type == Short)
            v.push_back(static_cast<T>(toShortInt()));
        else if(d->type == UShort)
            v.push_back(static_cast<T>(toUShortInt()));
        else if(d->type == Int)
            v.push_back(static_cast<T>(toInt()));
        else if(d->type == UInt)
            v.push_back(static_cast<T>(toUInt()));
        else if(d->type == LongInt)
            v.push_back(static_cast<T>(toLongInt()));
        else if(d->type == LongUInt)
            v.push_back(static_cast<T>(toULongInt()));
        else if(d->type == Float)
            v.push_back(static_cast<T>(toFloat()));
        else if(d->type == Double)
            v.push_back(static_cast<T>(toDouble()));
        else if(d->type == LongDouble)
            v.push_back(static_cast<T>(toLongDouble()));
        else if(d->type == String)
        {
            try
            {
                // try converting to long double
                long double ld = std::stold(toString());
                v.push_back(static_cast<T>(ld));
            }
            catch(const std::invalid_argument &) {
                perr("CuVariant.toVector: string \"%s\" to number conversion failed: invalid argument", toString().c_str());
                valid = false;
            }
            catch(const std::out_of_range &) {
                perr("CuVariant.toVector: string \"%s\" to number conversion failed: out of range", toString().c_str());
                valid = false;
            }
        }
        else
            valid = false;
    }
    return valid;
}


#endif // CUVARIANT_T_H
