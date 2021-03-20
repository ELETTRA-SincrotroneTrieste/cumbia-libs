#ifndef CUMATRIX_H
#define CUMATRIX_H

#include <vector>
#include <string>
#include <sstream>

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
    CuMatrix(const std::vector<T>& v, size_t nro, size_t nco) {
        d = new CuMatrixPrivate<T>(v, nro, nco);
        printf("CuMatrix const std::vector<T>& v, size_t nro, size_t nco  constructor d %p\n", d);
    }

    CuMatrix(const CuMatrix &other) {
        printf("CuMatrix from other: other.d = %p\n", other.d);
        d = new CuMatrixPrivate<T>(other.d->data, other.d->nrows, other.d->ncols);
    }

    CuMatrix(CuMatrix && other) {
        printf("CuMatrix from other (MOVE): other.d = %p\n", other.d);
        /* no new d here! */
        d = other.d;
        other.d = nullptr; /* don't delete */
        printf("CuMatrix from other (MOVE): this->d %p\n", d);
    }

    CuMatrix() {
        d = new CuMatrixPrivate<T>(std::vector<T>(), 0, 0);
        printf("CuMatrix empty constructor d %p\n", d);
    }

    CuMatrix *clone() const {
        return new CuMatrix(d->data, d->nrows, d->ncols);
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
            printf("operator MOVE = d is like other %p\n", d);
            other.d  = nullptr; /* don't delete */
            printf("operator MOVE = OTHER is NULL, d is %p\n", d);
        }
        return *this;
    }

    std::string repr() const {
        std::stringstream sst;
        for(size_t r = 0; r < d->nrows; r++) {
            for(size_t c = 0; c < d->ncols; c++) {
                sst << d->data[c + r * d->nrows] << std::string("\t");
            }
            sst << std::string("\n");
        }
        return sst.str();
    }

private:
    CuMatrixPrivate <T> *d;
};

#endif // CUMATRIX_H
