#ifndef CUDATA_H
#define CUDATA_H

#include <sys/types.h>

#include <cuvariant.h>

class CuDataPrivate;

class CuData
{
public:
    virtual ~CuData();

    CuData();

    CuData(const CuVariant &v);

    CuData(const std::string& key, const CuVariant &v);

    CuData(const CuData &other);

    CuData(CuData&& other);

    virtual CuData & operator=(const CuData& other);

    virtual CuData &operator =(CuData &&other);

    size_t size() const;

    CuVariant value() const;

    CuVariant value(const std::string &key) const;

    void add(const std::string& key, const CuVariant &value);

    bool containsKey(const std::string & key) const;

    CuVariant& operator [] (const std::string& key);

    const CuVariant& operator [] (const std::string& key) const;

    virtual bool operator ==(const CuData &other) const;

    virtual bool operator !=(const CuData &other) const;

    bool isEmpty() const;

    void print() const;

    std::string toString() const;

private:
    CuDataPrivate *d;

    void mCopyData(const CuData &other);
};

#endif // CUDATA_H
