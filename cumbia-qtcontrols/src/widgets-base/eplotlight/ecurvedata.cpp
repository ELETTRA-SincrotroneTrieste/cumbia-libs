#include "ecurvedata.h"
#include <QtDebug>

CurveData::CurveData():
    d_count(0)
{
  
}

void CurveData::removeFirstElements(int n)
{
  if(d_count >= n)
  {
//     printf("rimuovo primi %d elementi x[0]: %f e y[0]: %f\n", n, d_x[0], d_y[0]);
    d_x.remove(0, n);
    d_y.remove(0, n);
    d_count -= n;
  }
}

void CurveData::set(const QVector< double > &xData, const QVector< double > &yData)
{
  d_x = xData;
  d_y = yData;
  d_count = xData.size();
}

void CurveData::append(double *x, double *y, int count)
{
    int newSize = ( (d_count + count) / 10 + 1 ) * 10;
    if ( newSize > size() )
    {
        d_x.reserve(newSize);
        d_y.reserve(newSize);
    }

    for (int i = 0; i < count; i++ )
    {
        d_x.append(x[i]);
        d_y.append(y[i]);
    }
    d_count += count;
}

void CurveData::insert(double *x, double *y, int count, double default_y)
{
    size_t i = 0;
    int j = 0;
    size_t ds = d_x.size();
    while(i < ds && j < count) {
        while(j < count && x[j] <= d_x[i]) {
            if(x[j] == d_x[i]) {
                // replace y
                d_y[i] = m_get_yval(y, i, count, default_y);
            } else {
                d_x.insert(i, x[j]);
                d_y.insert(i, m_get_yval(y, j, count, default_y));
                d_count++;
            }
            j++;
        }
        i++;
        ds = d_x.size();
    }
    double yv = m_get_yval(y, j, count, default_y);
    double *yarr = new double[count - j];
    for(int i = 0; i < count - j; i++)
        yarr[i] = yv;
    append(x + j, yarr, count - j);
    delete yarr;
}

int CurveData::count() const
{
    return d_count;
}

int CurveData::size() const
{
    return d_x.size();
}

const double *CurveData::x() const
{
    return d_x.data();
}

const double *CurveData::y() const
{
    return d_y.data();
}

void CurveData::reserve(int newSize)
{
    d_x.reserve(newSize);
    d_y.reserve(newSize);
}

double CurveData::m_get_yval(double *y, int idx, int siz, double default_y) const {
    if(y != nullptr && idx < siz)
        return y[idx];
    int i = idx - 1;
    while(d_y.size() < i)
        i--;
    if(i > -1)
        return d_y[i];
    return default_y;
}
