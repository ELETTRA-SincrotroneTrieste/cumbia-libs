#ifndef CUVARIANT_T_H
#define CUVARIANT_T_H

#include "cuvariantprivate.h"
#include <cuvariant.h>

template<typename T> void CuVariant::to(T &val) const
{
    val = 0;
    d->mIsValid = true;
    if(d->format == Scalar)
    {
        switch(d->type)
        {
        case Short:
            val = *(static_cast<short *>(d->val));
            break;
        case UShort:
            val = *(static_cast<unsigned short *>(d->val));
            break;
        case Int:
            val = *(static_cast<int *>(d->val));
            break;
        case UInt:
            val = *(static_cast<unsigned int *>(d->val));
            break;

        case LongInt:
            val = *(static_cast<long int *>(d->val));
            break;
        case LongUInt:
            val = *(static_cast<unsigned long *>(d->val));
            break;
        case Float:
            val = *(static_cast<float *>(d->val));
            break;
        case Double:
            val = *(static_cast<double *>(d->val));
            break;
        case LongDouble:
            val = *(static_cast<long double *>(d->val));
            break;
        case Boolean:
            val = *(static_cast<bool *>(d->val));
            break;
        case String:
            d->mIsValid = false;
            perr("CuVariant.to: string to number conversion not supported. Use toString instead");
            break;
        default:
            d->mIsValid = false;
            break;
        }
        if(!d->mIsValid)
            perr("CuVariant.to: unsupported scalar conversion from type %s and format %s", dataTypeStr(d->type).c_str(),
                 dataFormatStr(d->format).c_str());
    }
}

template<typename T> void CuVariant::toVector(std::vector<T> &v) const
{
    d->mIsValid = true;
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
            d->mIsValid = false;
            perr("CuVariant.toVector: string to number conversion not supported. Use toStringVector instead");
        }
        else
            d->mIsValid = false;
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
        else if(d->type == String) {
            d->mIsValid = false;
            perr("CuVariant.toVector: string to number conversion not supported. Use toStringVector instead");
        }
        else
            d->mIsValid = false;
    }
}


#endif // CUVARIANT_T_H
