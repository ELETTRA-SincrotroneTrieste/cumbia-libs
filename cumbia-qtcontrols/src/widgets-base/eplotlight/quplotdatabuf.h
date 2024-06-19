#ifndef QUPLOTDATABUF_H
#define QUPLOTDATABUF_H

#include <qwt_series_data.h>

class QuPlotDataBuf : public QwtSeriesData< QPointF >
{
public:
    QuPlotDataBuf(size_t siz = 0);

    void init(size_t siz);
    double x0() const;
    double xN() const;

    // QwtSeriesData interface
public:
    size_t size() const;
    QPointF sample(size_t i) const;
    QRectF boundingRect() const;

    void move(const std::vector<double>& y);
    void set(const std::vector<double>& y);

    std::vector<double> x, y;

private:
};

#endif // QUPLOTDATABUF_H
