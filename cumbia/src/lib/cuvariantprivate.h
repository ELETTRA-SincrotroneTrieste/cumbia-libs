#ifndef CUVARIANTPRIVATE_H
#define CUVARIANTPRIVATE_H

#include <stdlib.h>
#include <atomic>


/*! @private */
class CuVariantPrivate
{
public:
    CuVariantPrivate(const CuVariantPrivate &other);
    CuVariantPrivate();
    ~CuVariantPrivate();
    size_t mSize;
    int format; // DataFormat
    int type; // DataType
    bool mIsValid;
    bool mIsNull;
    void * val;

    int ref() {
        return _r.fetch_add(1);
    }
    int unref() {
        return _r.fetch_sub(1);
    }
    int exchange(int desired) {
        return _r.exchange(desired);
    }
    int load() const {
        return _r.load();
    }

private:
    std::atomic<int> _r;

};

#endif // XVARIANTPRIVATE_H
