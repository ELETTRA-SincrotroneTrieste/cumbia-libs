#ifndef QUPLOTDATABUF_H
#define QUPLOTDATABUF_H

#include <qwt_series_data.h>

class QuPlotDataBufP;


class QuPlotDataBuf : public QwtSeriesData< QPointF >
{
public:
    QuPlotDataBuf(size_t siz = 0);
    virtual ~QuPlotDataBuf();

    void init(size_t siz);
    double x0() const;
    double xN() const;

    QPointF p(size_t i) const;

    size_t resize(size_t s);

    void move(const std::vector<double>& y);
    void set(const std::vector<double>& y);

    std::vector<double> x, y;

    // QwtSeriesData interface
public:
    size_t size() const;
    QPointF sample(size_t i) const;
    QRectF boundingRect() const;

private:
    QuPlotDataBufP *d;
};

#endif // QUPLOTDATABUF_H
