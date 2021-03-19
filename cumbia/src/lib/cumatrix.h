#ifndef CUMATRIX_H
#define CUMATRIX_H

#include <vector>
#include <string>

template<typename T>
class CuMatrixPrivate {
public:
    CuMatrixPrivate(const std::vector<T>& v, size_t nr = 0, size_t nc = 0) : data(v), nrows(nr), ncols(nc) {

    }

    ~CuMatrixPrivate() {
    }
    std::vector<T> data;
    size_t nrows, ncols;
};

template <typename T> class CuMatrix
{
public:
    CuMatrix(const std::vector<T>& v, size_t nro, size_t nco)
    {
        d = new CuMatrixPrivate<T>(v, nro, nco);
    }
    CuMatrix(const CuMatrix &other)
    {
        d = new CuMatrixPrivate<T>(other.d->data, other.d->nrows, other.d->ncols);
    }
    CuMatrix(CuMatrix && other)
    {
        /* no new d here! */
        d = other.d;
        other.d = nullptr; /* don't delete */
    }
    CuMatrix() {
        d = new CuMatrixPrivate<T>(std::vector<T>(), 0, 0);
    }

    virtual ~CuMatrix() {
        if(d) delete d;
    }

    size_t nrows() const {
        return d->nrows;
    }
    size_t ncols() const {
        return d->ncols;
    }

    std::vector<T> data() const {
        return d->data;
    }

    CuMatrix<T> &operator=(const CuMatrix &other) {
        if(this != &other) {
            if(d) delete d;
            d = new CuMatrixPrivate<T>(other.d->data, other.d->nrows, other.d->ncols);
        }
        return *this;
    }

    bool operator ==(const CuMatrix &other) const {
        return other.d->ncols == this->d.ncols && other.d->nrows == this->d->nrows &&
                d->data == other.d->data;
    }

    bool operator !=(const CuMatrix &other) const {
        return !operator ==(other);
    }

    CuMatrix<T> &operator=(CuMatrix &&other) {
        if(this != &other) {
            if(d) delete d;
            d = other.d;
            other.d  = nullptr; /* don't delete */
        }
        return *this;
    }

    std::string repr() const {
        std::string re;
        for(size_t r = 0; r < d->nrows; r++) {
            for(size_t c = 0; c < d->ncols; c++) {
                re += std::to_string(d->data[c + r * d->nrows]) + std::string("\t");
            }
            re += std::string("\n");
        }
        return re;
    }

private:
    CuMatrixPrivate <T> *d;
};

#endif // CUMATRIX_H
