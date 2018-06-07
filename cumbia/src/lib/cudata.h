#ifndef CUDATA_H
#define CUDATA_H

#include <sys/types.h>

#include <cuvariant.h>

#define DATA2CHUNK 32

class CuDType {
public:

    enum Key { Activity = 1,
               Id, ///< id
               Thread,  ///< thread
               Class,  ///< class
               Value, ///<
               Src,  ///<
               Name, ///< some name
               Err,  ///<
               Time,  ///< timestamp
               Time_ms,   ///< timestamp millis
               Time_ns,  ///< timestamp nanoseconds
               Time_us,  ///< timestamp microsecs
               Timestamp_Str, ///< timestamp as string
               Message, ///< a message
               Mode,  ///< a mode
               Type,  ///< some type
               Exit,  ///< exit flag
               Ptr,  ///< pointer to something (void* is supported by CuVariant)
               MaxBaseDataKey = 24,
               MaxDataKey = 64 };
};

class CuDTypeUtils
{
public:

    virtual ~CuDTypeUtils() {}

    virtual std::string keyName(int k) const {
        switch(k){
        case CuDType::Activity:
            return "Activity";
        case CuDType::Id:
            return "Id";
        case CuDType::Thread:
            return "Thread";
        case CuDType::Class:
            return "Class";
        case CuDType::Value:
            return std::string("Value");
        case CuDType::Src:
            return std::string("Src");
        case CuDType::Err:
            return std::string("Err");
        case CuDType::Ptr:
            return std::string("Ptr");
        case CuDType::Time:
            return std::string("Time");
        case CuDType::Time_ms:
            return std::string("Time_ms");
        case CuDType::Time_us:
            return std::string("Time_us");
        case CuDType::Time_ns:
            return std::string("Time_ns");

        case CuDType::Message:
            return std::string("Message");
        case CuDType::Mode:
            return std::string("Mode");
        case CuDType::Type:
            return std::string("Type");
        case CuDType::Exit:
            return std::string("Exit");

        case CuDType::MaxBaseDataKey:
            return std::string("MaxBaseDataKey");
        case CuDType::MaxDataKey:
            return std::string("MaxDataKey");
        default:
            return std::string("Unknown_key ") + std::to_string(k);
        }
    }
};

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

    CuData(const size_t key, const CuVariant &v);

    CuData(const CuData &other);

    CuData(CuData&& other);

    virtual CuData & operator=(const CuData& other);

    virtual CuData &operator =(CuData &&other);

    size_t stringmapsize() const;

    CuVariant value(const std::string &key) const;

    CuVariant value(const size_t key) const;

    void add(const std::string& key, const CuVariant &value);

    void add(const size_t key, const CuVariant &value);

    bool containsStrKey(const std::string & key) const;

    bool containsKey(const size_t key) const;

    CuVariant& operator [] (const std::string& key);

    CuVariant& operator [] (const size_t key);

    const CuVariant& operator [] (const std::string& key) const;

    const CuVariant& operator [] (const size_t key) const;

    virtual bool operator ==(const CuData &other) const;

    virtual bool operator !=(const CuData &other) const;

    bool isEmpty() const;

    void print() const;

    std::string toString() const;

    void putTimestamp();

private:
    CuDataPrivate *d;

    void mCopyData(const CuData &other);

    void m_insertDataExtra(int index, const CuVariant& v);
    void m_reallocData2(int index);
};

#endif // CUDATA_H
