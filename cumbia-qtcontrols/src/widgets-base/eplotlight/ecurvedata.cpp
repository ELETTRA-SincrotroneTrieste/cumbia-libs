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
    printf("CurveData.append: d_count %d count %d new size %d AFIQ!?\n", d_count, count, newSize);
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


#include <QDateTime>
#include <cumacros.h>
void CurveData::insert(double *x, double *y, int count)
{
    size_t i = 0;
    int j = 0;
    size_t ds = d_x.size();
    printf("\n>>> CurveData.insert: size b4 %d going to add %d values\n", d_x.size(), count);
    while(i < ds && j < count) {
        while(j < count && x[j] <= d_x[i]) {
            if(x[j] == d_x[i]) {
                // replace y
                d_y[i] = y[j];
            } else {
                d_x.insert(i, x[j]);
                d_y.insert(i, y[j]);
                d_count++;
            }
            j++;
        }
        i++;
        ds = d_x.size();
    }
    printf("CurveData.insert: now appending %d values count %d j %d \n", count - j, count, j);
    append(x + j, y + j, count - j);
    for(int i = 0; i < d_x.size(); i++) {
        printf("\e[1;33m%s: \e[1;32m%f\e[0m\n", qstoc(QDateTime::fromMSecsSinceEpoch(d_x[i]).toString()), d_y[i]);
    }
    printf("\n>>> CurveData.insert: size afta %d\n", d_x.size());
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







