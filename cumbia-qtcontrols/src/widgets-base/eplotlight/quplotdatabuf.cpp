#include "quplotdatabuf.h"
#include <cumacros.h>

class QuPlotDataBufP {
public:
    QuPlotDataBufP() : first(0), x_auto(true) {}
    size_t bufsiz, first, datasiz;
    bool x_auto;
};

QuPlotDataBuf::QuPlotDataBuf(size_t siz) {
    d = new QuPlotDataBufP();
    init(siz);
}

QuPlotDataBuf::~QuPlotDataBuf() {
    delete d;
}

/*!
 * \brief intialize x with values from 0 to siz - 1, y with zeroes
 * \param siz the *buffer* size
 */
void QuPlotDataBuf::init(size_t bufsiz) {
    y.resize(bufsiz, 0);
    d->bufsiz = bufsiz;
    d->datasiz = d->first = 0;
}

double QuPlotDataBuf::x0() const {
    return d->x_auto ? d->first : p(0).x();
}

double QuPlotDataBuf::xN() const {
    const QPointF& x = p(d->datasiz - 1);
    return x.x();
}

bool QuPlotDataBuf::x_auto() const {
    return d->x_auto;
}

size_t QuPlotDataBuf::first() const {
    return d->first;
}

QPointF QuPlotDataBuf::p(size_t i) const {
    QPointF r(-1.0, -1.0);
    if(i >= d->datasiz)
        return r;
    size_t idx = (d->first + i) % d->bufsiz;
    return QPointF(d->x_auto ? i : x[idx], y[idx]);
}

double QuPlotDataBuf::py(size_t i) const {
    if(i >= d->datasiz)
        return -1;
    size_t idx = (d->first + i) % d->bufsiz;
    return  y[idx];
}

size_t QuPlotDataBuf::size() const {
    return d->datasiz;
}

size_t QuPlotDataBuf::bufsize() const {
    return d->bufsiz;
}

QPointF QuPlotDataBuf::sample(size_t i) const {
    return d->x_auto ?  QPointF(i, py(i)) : p(i);
}

QRectF QuPlotDataBuf::boundingRect() const {
    double x = 0, y = 0, w = 100, h = 1000;
    pretty_pri("rect (%.1f,%.1f %.1fx%.1f\n", x, y , w, h);
    return QRectF(x, y, w, h);
}

/*!
 * \brief resizes the *buffer* to new size s.
 *
 * If new size < old size, the *tail* of data is preserved
 *
 * \return new size - old size
 */
size_t QuPlotDataBuf::resize(size_t s) {
    size_t oldsiz(d->bufsiz);
    std::vector<double> X, Y;
    if(s >= d->bufsiz) {
        // re-arrange elements so that d->first is 0
        X.resize(d->x_auto ? 0 : d->datasiz, 0);
        Y.resize(d->datasiz, 0);
        for(size_t i = d->first, j = 0; i < d->datasiz; i++, j++) {
            if(!d->x_auto)
                X[j] = x[i % d->bufsiz];
            Y[j] = y[i % d->bufsiz];
        }
    }
    else { // smaller size: preserve tail
        X.resize(d->x_auto ? 0 : s, 0);
        Y.resize(std::min(s, d->datasiz), 0);
        // save tail into Y (X)
        for(int i = d->datasiz - 1, j = Y.size() - 1; j >= 0 && i >= 0; i--, j--) {
            const QPointF& xy = p(i);
            if(!d->x_auto)
                X[j] = xy.x();
            Y[j] = xy.y();
        }
        d->datasiz = Y.size();
    }
    if(!d->x_auto)
        x = std::move(X);
    y = std::move(Y);
    d->bufsiz = s;
    d->first = 0;
    return d->bufsiz - oldsiz;
}

/*! \brief move _y into internal data y
 * \param _y data to be moved using std::move
 *
 * \par Important note
 * Use *move* only when you intend to use QuPlotDataBuf as a spectrum data storage
 *
 * \note y is public and can be changed directly when you never intend to use
 *       QuPlotDataBuf as a circular buffer
 */
void QuPlotDataBuf::move(const std::vector<double> &_y) {
    y = std::move(_y);
}

/*!
 * \brief copy _y into y
 * \param _y data to be copied
 *
 * \note  y is a public variable that can be directly set
 * \note x_auto property is set to true when explicitly setting the y array
 */
void QuPlotDataBuf::set(const std::vector<double> &_y) {
    y = _y;
    d->datasiz = d->bufsiz = y.size();
    d->first = 0;
    d->x_auto = true;
}

/*!
 * \brief copy from xx and yy
 * \param xx source for the x data
 * \param yy source for the y data
 *
 * This method sets x_auto to false: xx data shall be used as custom x axis coordinates
 * (e.g. timestamps). Resets datasiz and bufsiz to xx.size() (which shall be equal to yy.size()).
 * In case of xx and yy sizes mismatch, no operation shall be done
 *
 * To leave this method efficient and general purpose, in the case you want to initialize
 * x and y with xx and yy and then *append* new data, call set and then resize to the
 * desired total buffer size. Then append.
 */
void QuPlotDataBuf::set(const std::vector<double> &xx, const std::vector<double> &yy) {
    if(xx.size() == yy.size()) {
        y = yy;
        x = xx;
        d->x_auto = false;
        d->first = 0;
        d->datasiz = d->bufsiz = xx.size();
    }
}

void QuPlotDataBuf::append(double *xx, double *yy, size_t count) {
    size_t next = (d->first + d->datasiz) % d->bufsiz;
    for(size_t i = 0; i < count; i++ ) {
        x[next] = xx[i];
        y[next] = yy[i];
        if(d->datasiz < d->bufsiz)
            d->datasiz++;
        else
            d->first = (d->first + 1) % d->bufsiz;
        next = (next + 1) % d->bufsiz;
    }
    d->x_auto = false;
}

void QuPlotDataBuf::append(double *yy, size_t count) {
    if(d->x_auto) {
        size_t next = (d->first + d->datasiz) % d->bufsiz;
        for(size_t i = 0; i < count; i++ ) {
            y[next] = yy[i];
            if(d->datasiz < d->bufsiz)
                d->datasiz++;
            else
                d->first = (d->first + 1) % d->bufsiz;
            next = (next + 1) % d->bufsiz;
        }
    }
}

/*!
 * \brief insert values at pos idx
 *
 * y is extended by inserting new elements before the element at the
 * specified position, increasing the data size by the number of
 * elements inserted.
 *
 * \param idx position where to insert. If greater than *size*, then
 *        data shall be appended at the end. If less than 0, it shall
 *        be inserted before current data.
 * \param yy pointer to data
 * \param count number of elements in yy
 */
void QuPlotDataBuf::insert(size_t idx, double *yy, size_t count) {
    if(d->x_auto) {
        if(idx > d->datasiz) idx = d->datasiz;
        if(idx < 0) idx = 0;
        if(d->bufsiz >= d->datasiz + count) {
            // there is enough space for data: use std vector insert
            y.insert(y.begin() + idx, yy, yy + count);
            d->datasiz += count;
        } else {
            // preserve the tail: map idx
            size_t tlen = d->datasiz - idx;
            std::vector<double> Y(tlen);
            for(size_t i = 0; i < tlen; i++) {
                size_t j = (d->first + i + idx) % d->bufsiz;
                Y[i] = y[j];
            } // Y contains the tail
            size_t next = (d->first + idx) % d->bufsiz;
            // append yy starting from idx (datasiz)
            for(size_t i = 0; i < count; i++ ) {
                y[next] = yy[i];
                idx < d->bufsiz ? idx++ :
                    d->first = (d->first + 1) % d->bufsiz;
                next = (next + 1) % d->bufsiz;
            }
            // append saved tail
            for(size_t i = 0; i < tlen; i++) {
                y[next] = Y[i];
                idx < d->bufsiz ? idx++ :
                    d->first = (d->first + 1) % d->bufsiz;
                next = (next + 1) % d->bufsiz;
            }
            d->datasiz = idx;
        }
    }
}

void QuPlotDataBuf::insert(size_t idx, double *xx, double *yy, size_t count)
{
    if(!d->x_auto) {
        if(idx > d->datasiz) idx = d->datasiz;
        if(idx < 0) idx = 0;
        if(d->bufsiz >= d->datasiz + count) {
            // there is enough space for data: use std vector insert
            y.insert(y.begin() + idx, yy, yy + count);
            x.insert(x.begin() + idx, xx, xx + count);
            d->datasiz += count;
        } else {
            // preserve the tail: map idx
            size_t tlen = d->datasiz - idx;
            std::vector<double> Y(tlen), X(tlen);
            for(size_t i = 0; i < tlen; i++) {
                size_t j = (d->first + i + idx) % d->bufsiz;
                Y[i] = y[j];
                X[i] = x[j];
            } // Y contains the tail
            size_t next = (d->first + idx) % d->bufsiz;
            // append yy starting from idx (datasiz)
            for(size_t i = 0; i < count; i++ ) {
                y[next] = yy[i];
                x[next] = xx[i];
                idx < d->bufsiz ? idx++ :
                    d->first = (d->first + 1) % d->bufsiz;
                next = (next + 1) % d->bufsiz;
            }
            // append saved tail
            for(size_t i = 0; i < tlen; i++) {
                y[next] = Y[i];
                x[next] = xx[i];
                idx < d->bufsiz ? idx++ :
                    d->first = (d->first + 1) % d->bufsiz;
                next = (next + 1) % d->bufsiz;
            }
            d->datasiz = idx;
        }
    }
}
