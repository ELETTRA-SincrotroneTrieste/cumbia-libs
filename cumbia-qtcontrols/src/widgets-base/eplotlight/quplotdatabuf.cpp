#include "quplotdatabuf.h"
#include <cumacros.h>

class QuPlotDataBufP {
public:
    QuPlotDataBufP(size_t s) : bufsiz(s), first(0), datasiz(0) {}
    size_t bufsiz, first, datasiz;
};

QuPlotDataBuf::QuPlotDataBuf(size_t siz) {
    d = new QuPlotDataBufP(siz);
    if(siz > 0) {
        x.resize(siz, 0);
        y.resize(siz, 0);
    }
}

QuPlotDataBuf::~QuPlotDataBuf() {
    delete d;
}

void QuPlotDataBuf::init(size_t siz) {
    x.resize(siz, 0);
    y.resize(siz, 0);
    d->bufsiz = siz;
    d->datasiz = d->first = 0;
}

double QuPlotDataBuf::x0() const {
    const QPointF& x = p(0);
    return x.x();
}

double QuPlotDataBuf::xN() const {
    const QPointF& x = p(d->datasiz - 1);
    return x.x();
}

QPointF QuPlotDataBuf::p(size_t i) const {
    QPointF r(-1.0, -1.0);
    if(i >= d->datasiz)
        return r;
    size_t idx = (d->first + i) % d->datasiz;
    return QPointF(x[idx], y[idx]);
}

size_t QuPlotDataBuf::size() const {
    return d->datasiz;
}

QPointF QuPlotDataBuf::sample(size_t i) const {
    return QPointF(x[i], y[i]);
}

QRectF QuPlotDataBuf::boundingRect() const {
    double x = 0, y = 0, w = 100, h = 1000;
    pretty_pri("rect (%.1f,%.1f %.1fx%.1f\n", x, y , w, h);
    return QRectF(x, y, w, h);
}

/*!
 * \brief resizes to new size s.
 * \return new size - old size
 */
size_t QuPlotDataBuf::resize(size_t s) {
    size_t oldsiz(d->bufsiz);
    if(s > d->bufsiz) {
        // resize buffers, datasiz unchanged
        x.resize(s, 0);
        y.resize(s, 0);
    } else { // smaller size
        std::vector<double> X, Y;
        X.resize(s);
        Y.resize(s);
        d->datasiz = s; // smaller datasiz
        for(size_t i = oldsiz - s, j = 0; i < oldsiz; i++, j++) {
            const QPointF& xy = p(i);
            X[j] = xy.x();
            Y[j] = xy.y();
        }
        x = std::move(X);
        y = std::move(Y);
    }
    d->bufsiz = s; // new siz
    return d->bufsiz - oldsiz;
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
