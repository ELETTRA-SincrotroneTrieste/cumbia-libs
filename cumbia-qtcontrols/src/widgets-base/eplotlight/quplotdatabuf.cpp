#include "quplotdatabuf.h"
#include <cumacros.h>

class QuPlotDataBufP {
public:
    QuPlotDataBufP() : first(0),
        xax_auto(true), xb_auto(false), yb_auto(false) {}
    size_t bufsiz, first, datasiz;
    bool xax_auto; // x axis auto populate
    bool xb_auto, yb_auto; // track x and y data bounds
    double yb[2];
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
    d->yb[0] = 1.0; d->yb[1] = -1.0; // min > max forces initialization
}

double QuPlotDataBuf::x0() const {
    return d->xax_auto ? d->first : p(0).x();
}

double QuPlotDataBuf::xN() const {
    return d->datasiz > 0 ? p(d->datasiz - 1).x() : -1;
}

bool QuPlotDataBuf::x_auto() const {
    return d->xax_auto;
}

size_t QuPlotDataBuf::first() const {
    return d->first;
}

QPointF QuPlotDataBuf::p(size_t i) const {
    QPointF r(-1.0, -1.0);
    if(i >= d->datasiz)
        return r;
    size_t idx = (d->first + i) % d->bufsiz;
    return QPointF(d->xax_auto ? i : x[idx], y[idx]);
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
    return d->xax_auto ?  QPointF(i, py(i)) : p(i);
}

QRectF QuPlotDataBuf::boundingRect() const {
    double x0 = d->xb_auto ? (x.size() > 0 && d->datasiz > 0 ? x[0] : 0) : 0,
        y0 = d->yb[0] == d->yb[1] ? 0 : d->yb[0],
        w = d->xb_auto ? (x.size() > 0 && d->datasiz > 0 ? x[d->datasiz - 1] - x[0] : 1000) : 1000,
        h = d->yb[0] == d->yb[1] ? 1000 : d->yb[1] - d->yb[0];
    pretty_pri("rect (%.1f,%.1f %.1fx%.1f\n", x0, y0 , w, h);
    return QRectF(x0, y0, w, h);
}

/*!
 * \brief resizes the *buffer* to new size s.
 *
 * If new size < old size, the *tail* of data is preserved
 *
 * \return new size - old size
 */
size_t QuPlotDataBuf::resizebuf(size_t s) {
    size_t oldsiz(d->bufsiz);
    std::vector<double> X, Y;
    if(s >= d->bufsiz) {
        // re-arrange elements so that d->first is 0
        X.resize(d->xax_auto ? 0 : d->datasiz, 0);
        Y.resize(d->datasiz, 0);
        for(size_t i = 0, j = 0; i < d->datasiz; i++, j++) {
            if(!d->xax_auto)
                X[j] = x[d->first + i % d->bufsiz];
            Y[j] = y[d->first + i % d->bufsiz];
        }
    }
    else { // smaller size: preserve tail
        X.resize(d->xax_auto ? 0 : s, 0);
        Y.resize(std::min(s, d->datasiz), 0);
        // save tail into Y (X)
        for(int i = d->datasiz - 1, j = Y.size() - 1; j >= 0 && i >= 0; i--, j--) {
            const QPointF& xy = p(i);
            if(!d->xax_auto)
                X[j] = xy.x();
            Y[j] = xy.y();
        }
        d->datasiz = Y.size();
    }
    if(!d->xax_auto)
        x = std::move(X);
    y = std::move(Y);
    if(s >= d->bufsiz) {
        if(!d->xax_auto) x.resize(s);
        y.resize(s);
    }
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
    if(d->datasiz != y.size())
        d->datasiz = y.size();
    if(d->bufsiz != d->datasiz)
        d->bufsiz = d->datasiz;

    if(d->yb_auto) { // must find min and max in yy
        auto [a, b] = std::minmax_element(y.begin(), y.end());
        d->yb[0] = *a.base();
        d->yb[1] = *b.base();
    }
}

void QuPlotDataBuf::move(const std::vector<double> &_x, const std::vector<double> &_y) {
    if(_x.size() == _y.size()) {
        x = std::move(_x);
        if(!d->xax_auto)
            d->xax_auto = true;
        if(d->datasiz != y.size())
            d->datasiz = y.size();
        if(d->bufsiz != d->datasiz)
            d->bufsiz = d->datasiz;

        if(d->yb_auto) { // must find min and max in yy
            auto [a, b] = std::minmax_element(y.begin(), y.end());
            d->yb[0] = *a.base();
            d->yb[1] = *b.base();
        }
    }
}

/*!
 * \brief copy _y into y
 * \param _y data to be copied
 *
 * \note  y is a public variable that can be directly set
 * \note xax_auto property is set to true when explicitly setting the y array
 */
void QuPlotDataBuf::set(const std::vector<double> &_y) {
    y = _y;
    d->datasiz = d->bufsiz = y.size();
    d->first = 0;
    d->xax_auto = true;
    if(d->yb_auto) { // must find min and max in yy
        auto [a, b] = std::minmax_element(y.begin(), y.end());
        d->yb[0] = *a.base();
        d->yb[1] = *b.base();
    }
}

/*!
 * \brief copy from xx and yy
 * \param xx source for the x data
 * \param yy source for the y data
 *
 * This method sets xax_auto to false: xx data shall be used as custom x axis coordinates
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
        if(!d->xax_auto)  d->xax_auto = false;
        if(d->first != 0) d->first = 0;
        d->datasiz = d->bufsiz = x.size();
        if(d->yb_auto) { // must find min and max in yy
            auto [a, b] = std::minmax_element(yy.begin(), yy.end());
            d->yb[0] = *a.base();
            d->yb[1] = *b.base();
        }
    }
}

void QuPlotDataBuf::append(double *xx, double *yy, size_t count) {
    if(d->datasiz == 0)
        x.resize(y.size());
    size_t next = (d->first + d->datasiz) % d->bufsiz;
    for(size_t i = 0; i < count; i++ ) {
        x[next] = xx[i];
        y[next] = yy[i];
        if(d->datasiz < d->bufsiz)
            d->datasiz++;
        else
            d->first = (d->first + 1) % d->bufsiz;
        next = (next + 1) % d->bufsiz;

        if(d->yb_auto) {
            if(d->yb[0] > d->yb[1])
                d->yb[0] = d->yb[1] = yy[i];
            else if(yy[i] < d->yb[0] )
                d->yb[0] = yy[i];
            else if(yy[i] > d->yb[1])
                d->yb[1] = yy[i];
            pretty_pri("\e[1;32myb %.1f  yub %.1f\e[0m", d->yb[0], d->yb[1]);
        }
    }
    d->xax_auto = false;
}

void QuPlotDataBuf::append(double *yy, size_t count) {
    if(d->xax_auto) {
        size_t next = (d->first + d->datasiz) % d->bufsiz;
        for(size_t i = 0; i < count; i++ ) {
            y[next] = yy[i];
            if(d->datasiz < d->bufsiz)
                d->datasiz++;
            else
                d->first = (d->first + 1) % d->bufsiz;
            next = (next + 1) % d->bufsiz;
            // update y bounds if necessary
            if(d->yb_auto) {
                if(d->yb[0] > d->yb[1])
                    d->yb[0] = d->yb[1] = yy[i];
                else if(yy[i] < d->yb[0] )
                    d->yb[0] = yy[i];
                else if(yy[i] > d->yb[1])
                    d->yb[1] = yy[i];
            }
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
    if(d->xax_auto) {
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
                // update y bounds if necessary
                if(d->yb_auto) {
                    if(d->yb[0] > d->yb[1])
                        d->yb[0] = d->yb[1] = yy[i];
                    else if(yy[i] < d->yb[0] )
                        d->yb[0] = yy[i];
                    else if(yy[i] > d->yb[1])
                        d->yb[1] = yy[i];
                }
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
    if(!d->xax_auto) {
        if(idx > d->datasiz) idx = d->datasiz;
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
                // update y bounds if necessary
                if(d->yb_auto) {
                    if(d->yb[0] > d->yb[1])
                        d->yb[0] = d->yb[1] = yy[i];
                    else if(yy[i] < d->yb[0] )
                        d->yb[0] = yy[i];
                    else if(yy[i] > d->yb[1])
                        d->yb[1] = yy[i];
                }
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

void QuPlotDataBuf::setBoundsAuto(bool x, bool y) {
    d->xb_auto = x;
    d->yb_auto = y;
    if(y) { // reset min > max to force first initialization
        d->yb[0] = 1.0; d->yb[1] = -1.0;
    }
}

bool QuPlotDataBuf::xBoundsAuto() {
    return d->xb_auto;
}

bool QuPlotDataBuf::yBoundsAuto() {
    return d->yb_auto;
}


