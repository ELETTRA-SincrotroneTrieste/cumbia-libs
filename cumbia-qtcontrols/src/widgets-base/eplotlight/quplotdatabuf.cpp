#include "quplotdatabuf.h"
#include <cumacros.h>

QuPlotDataBuf::QuPlotDataBuf(size_t siz) {
    if(siz > 0)
        init(siz);
}

void QuPlotDataBuf::init(size_t siz) {
    x.resize(siz);
    for(size_t i = 0; i < siz; i++)
        x[i] = i;
}

double QuPlotDataBuf::x0() const {
    return (x.size() > 0) ? x[0] : 0.0;
}

double QuPlotDataBuf::xN() const {
    return (x.size() > 0) ? x[x.size() - 1] : 0.0;
}

size_t QuPlotDataBuf::size() const {
    return x.size() == y.size() ? x.size() : 0;
}

QPointF QuPlotDataBuf::sample(size_t i) const {
    return QPointF(x[i], y[i]);
}

QRectF QuPlotDataBuf::boundingRect() const {
    double x = 0, y = 0, w = 100, h = 1000;
    pretty_pri("rect (%.1f,%.1f %.1fx%.1f\n", x, y , w, h);
    return QRectF(x, y, w, h);
}

/*! \brief move _y into internal data y
 * \param _y data to be moved using std::move
 *
 * \note y is public and can be changed directly
 */
void QuPlotDataBuf::move(const std::vector<double> &_y) {
    y = std::move(_y);
}

/*!
 * \brief copy _y into y
 * \param _y data to be copied
 *
 * \note  y is a public variable that can be directly set
 */
void QuPlotDataBuf::set(const std::vector<double> &_y) {
    y = _y;
}
