#include "quvaluefilter.h"

QuValueFilter::QuValueFilter()
{

}

void QuValueFilter::filter(const CuData& , int &, bool , State )
{

}

void QuValueFilter::filter(const CuData &, short &, bool , QuValueFilter::State )
{

}

void QuValueFilter::filter(const CuData& , unsigned int &, bool , State )
{

}

void QuValueFilter::filter(const CuData& , double &, bool, State )
{

}

void QuValueFilter::filter(const CuData& , QString &, bool , State )
{

}

void QuValueFilter::filter(const CuData& , bool &, bool , State )
{

}

void QuValueFilter::filter(const CuData& , QVector<bool> &, bool , State )
{

}

void QuValueFilter::filter(const CuData& , QVector<double> &, bool , State )
{

}


void QuValueFilter::filter(const CuData& , QVector<int> &, bool, State )
{

}


void QuValueFilter::filter(const CuData& , QStringList &, bool , State )
{

}

void QuValueFilter::filter(const CuData &in_d, CuData &out_d, bool read, State updateState) {
    Q_UNUSED(in_d); Q_UNUSED(out_d); Q_UNUSED(read); Q_UNUSED(updateState);
}
