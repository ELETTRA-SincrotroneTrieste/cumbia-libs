#ifndef CUDATA_H
#define CUDATA_H

#include <sys/types.h>

#include <cuvariant.h>

class CuDataPrivate;

/*! \brief container for data exchange for the *cumbia library* and its clients
 *
 * CuData is a bundle that stores key-value pairs. The key is a std::string, the
 * value is of type CuVariant. A copy of the value associated to a key can be
 * obtained with CuData::value(const std::string& key). A value can be accessed
 * with the *[]* operator. *[]* operators are defined in the const and non const
 * flavours. The non const version allows to insert new CuVariant values with a
 * *string* key within the square brackets.
 *
 * The parameterless CuData::value version returns the value associated to an
 * *empty key*, if there is one, created for example with the
 * CuData::CuData(const CuVariant &v) constructor.
 *
 * The number of key-value pairs stored by CuData can be queried with CuData::size.
 * CuData::isEmpty returns true if the CuData has no key/value pair.
 * CuData::containsKey tells whether a given *key* is or not in the bundle.
 * CuData::add can be used to add a key-value pair. The result is equivalent to
 * using the *[]* operator.
 *
 * Two CuData can be compared with the == and != operators. The == operator returns
 * true if the two bundles contain the same keys and each of their values correspond.
 * The operator != returns the opposite value of the operator ==
 *
 * The CuData::toString method returns a std::string representation of its contents.
 * The contents of a CuData can be printed on the console by means of CuData::print
 *
 * \par The CuVariant variant type
 * Please read the CuVariant documentation to understand the *cumbia* CuVariant type.
 */
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

    CuData& set(const std::string& key, const CuVariant& value);

    CuData& merge(const CuData& other);

    CuData& remove(const std::string& key);

    CuData remove(const std::string& key) const;

    CuData& remove(const std::vector<std::string>& keys);

    CuData remove(const std::vector<std::string>& keys) const;

    size_t size() const;

    CuVariant value() const;

    CuVariant value(const std::string &key) const;

    void add(const std::string& key, const CuVariant &value);

    bool containsKey(const std::string & key) const;

    bool has(const std::string &key, const std::string& value) const;

    CuVariant& operator [] (const std::string& key);

    const CuVariant& operator [] (const std::string& key) const;

    virtual bool operator ==(const CuData &other) const;

    virtual bool operator !=(const CuData &other) const;

    bool isEmpty() const;

    void print() const;

    std::string toString() const;

    void putTimestamp();

    std::vector<std::string> keys() const;

    // shortcuts to extract data
    std::string s(const std::string& key) const;
    double d(const std::string& key) const;
    int i(const std::string& key) const;
    unsigned int u(const std::string& key) const;
    bool b(const std::string& key) const;

    // to<T> version shortcuts
    double D(const std::string& key) const;
    int I(const std::string& key) const;
    unsigned int U(const std::string& key) const;
    bool B(const std::string& key) const;

    // toVector<T> version shortcuts
    std::vector<double> DV(const std::string& key) const;
    std::vector<int>  IV(const std::string& key) const;
    std::vector<long long> LLV(const std::string &key) const;
    std::vector<unsigned int> UV(const std::string& key) const;
    std::vector<unsigned long int> ULV(const std::string& key) const;
    std::vector<unsigned long long int> ULLV(const std::string& key) const;
    std::vector<bool>    BV(const std::string& key) const;

private:
    CuDataPrivate *d_p;

    void mCopyData(const CuData &other);
};

#endif // CUDATA_H
