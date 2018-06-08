#include "cudata.h"
#include "cumacros.h"
#include <string>
#include <sys/time.h>
#include <unordered_map>
#include <map>
#include <set>


/*! @private */
class CuDataPrivate
{
public:
    CuDataPrivate() {
        datamap = NULL;
        data2 = NULL;
        data2siz = 0;
    }

    ~CuDataPrivate() {
        if(datamap)
            delete datamap;
        if(data2)
            delete [] data2;
    }

    CuVariant data[CuDType::MaxDataKey];

    // extra container
    CuVariant *data2;
    size_t data2siz;

    // string key version
    std::map<const std::string, CuVariant> *datamap;

    // set is an ordered container that records the keys
    // that have been inserted into CuData
    std::set<size_t> idx_set;

    CuVariant emptyVariant;
};

/*! \brief the class destructor
 *
 * CuData class destructor
 */
CuData::~CuData()
{
    if(d)
        delete d;
}

/*! \brief constructor of an empty CuData
 *
 * builds a CuData with no key/value
 */
CuData::CuData()
{
    d = new CuDataPrivate();
}

/*! \brief constructor initialising a CuData with one key-value pair
 *
 * @param key a string used as a key
 * @param v the value, as CuVariant
 *
 * The new CuData will be initialised with a key/v pair
 */
CuData::CuData(const size_t key, const CuVariant &v)
{
    d = new CuDataPrivate();
    if(key < CuDType::MaxDataKey)
        d->data[key] = v;
    else
        m_insertDataExtra(key, v);
    d->idx_set.insert(key);
}

/*! \brief the copy constructor
 *
 * @param other another CuData used as the source of the copy
 *
 * copies all the contents from the *other* CuData into the
 * new object
 */
CuData::CuData(const CuData &other)
{
    d = new CuDataPrivate();
    mCopyData(other);
}

/*! \brief c++11 *move constructor*
 *
 * @param other another CuData used as the source of the move operation
 *
 * Contents of *other* are moved into *this* CuData
 */
CuData::CuData(CuData &&other)
{
    d = other.d; /* no d = new here */
    other.d = NULL; /* avoid deletion! */
}

/*! \brief assignment operator, copies data from another source
 *
 * @param other another CuData which values will be copied into this
 */
CuData &CuData::operator=(const CuData &other)
{
    if(this != &other) {
        if(d->datamap)
            d->datamap->clear();
        mCopyData(other);
    }
    return *this;
}

/*! \brief move assignment operator, moves contents from another source
 *
 * @param other another CuData which values will be moved into this
 */
CuData &CuData::operator=(CuData &&other)
{
    if (this!=&other)
    {
        if(d)
            delete d;
        d = other.d;
        other.d = NULL; /* avoid deletion! */
    }
    return *this;
}

/** \brief returns the number of key-value pairs stored
 *
 * @return the number of key-value pairs stored within this object
 *
 * @see isEmpty
 */
size_t CuData::stringmapsize() const
{
    if(!d->datamap)
        return 0;
    return d->datamap->size();
}

/*! \brief return a copy of the value associated with the given *key*,
 *         or an *invalid and null* CuVariant
 *
 * @return a copy of the value associated with the given *key*,
 *         or an *invalid and null* CuVariant. See CuVariant::isValid and
 *         CuVariant::isNull
 *
 * See also CuData::d->datamap-> [](const std::string &key) const
 */
CuVariant CuData::value(const std::string & key) const
{
    if(d->datamap && d->datamap->count(key) > 0)
        return d->datamap->operator[](key);
    return CuVariant();
}

CuVariant CuData::value(const size_t key) const
{
    if(key < CuDType::MaxDataKey)
        return d->data[key];
    else if(d->data2 && key - CuDType::MaxDataKey < d->data2siz)
        return d->data2[key - CuDType::MaxDataKey];
    perr("CuData.value(key): key %ld both exceeds %d and %ld", key, CuDType::MaxDataKey, CuDType::MaxDataKey + d->data2siz);
    return CuVariant();
}

/*! \brief insert the key/value into the bundle
 *
 * @param key the new key, as a std::string
 * @param value the new value bound to *key*
 *
 * \note The effect is exactly the same as using the operator [] (const std::string& key)
 */
void CuData::add(const std::string & key, const CuVariant &value)
{
    if(!d->datamap)
        d->datamap = new std::map<const std::string, CuVariant>();
    d->datamap->operator [](key) = value;
}

void CuData::add(const size_t key, const CuVariant &value)
{
    if(key < CuDType::MaxDataKey)
        d->data[key] = value;
    else
        m_insertDataExtra(key, value);
    d->idx_set.insert(key);
}

/*! \brief returns true if the bundle contains the given key
 *
 * @param key the key to be searched, std::string
 * @return true if the bundle contains the given key, false otherwise
 */
bool CuData::containsStrKey(const std::string &key) const
{
    return d->datamap != NULL && d->datamap->count(key) > 0;
}

bool CuData::containsKey(const size_t key) const
{
    return d->idx_set.find(key) != d->idx_set.end();
}

/*! \brief array subscript write operator; pushes a new key-value pair into the bundle
 *
 * Use the insert operator to add a new key/value pair to the bundle
 *
 * \par  Example
 * \code
    CuData at("src", source.getName()); // string
    at[CuXDType::Device] = source.getDeviceName(); // string
    at[CuDType::Activity] = "event"; // string
    at[CuDType::Period] = source.period(); // integer
    at[CuDType::Err] = false; // bool
 * \endcode
 */
CuVariant &CuData::operator [](const std::string &key)
{
    if(!d->datamap)
        d->datamap = new std::map<const std::string, CuVariant>();
    return d->datamap->operator [](key);
}


CuVariant &CuData::operator [](const size_t key)
{
    d->idx_set.insert(key);
    if(key < CuDType::MaxDataKey)
        return d->data[key];
    else {
        size_t idx_offset = key - CuDType::MaxDataKey;
        if(idx_offset >= d->data2siz) {
            pgreentmp("CuData.operator [] (insert): key %ld both exceeds %d and %ld. reallocating", key, CuDType::MaxDataKey, CuDType::MaxDataKey + d->data2siz);
            m_reallocData2(idx_offset);
        }
        return d->data2[idx_offset];
    }
}

/*! \brief array subscript read operator: get a reference to a value given the key
 *
 * @param key the key to search for
 * @return reference to the value for the given key or a reference to an
 *         *empty* CuVariant if the key is not found.
 * \note An *empty* CuVariant::isValid method returns false and CuVariant::isNull returns true
 */
const CuVariant &CuData::operator [](const std::string &key) const
{
    if(d->datamap && d->datamap->count(key) > 0)
        return d->datamap->operator [](key);
    return d->emptyVariant;
}

const CuVariant &CuData::operator [](const size_t key) const
{
    if(key < CuDType::MaxDataKey)
        return d->data[key];
    else if(d->data2 && key - CuDType::MaxDataKey < d->data2siz) {
        return d->data2[key - CuDType::MaxDataKey];
    }
    return d->emptyVariant;
}


/*! \brief *equality* relational operator. Returns true if *this* CuData
 *         equals another
 *
 * @param other another CuData (const reference)
 * @return true if other contains the same keys as this and every key
 *         of other has the same value as the corresponding key in this
 *
 * the *inequality* operator is also defined
 */
bool CuData::operator ==(const CuData &other) const
{
    if( (d->datamap == NULL && other.d->datamap != NULL )
            || (d->datamap != NULL && other.d->datamap == NULL) )
        return false;

    if(other.d->datamap && d->datamap && other.d->datamap->size() != d->datamap->size())
        return false;

    if(other.d->datamap && d->datamap) {
        std::map<std::string, CuVariant>::const_iterator i;
        for(i = d->datamap->begin(); i != d->datamap->end(); ++i)
        {
            if(!other.containsStrKey(i->first))
                return false;
            if(other[i->first] != i->second)
                return false;
        }
    }
    if(d->idx_set.size() != other.d->idx_set.size() || d->idx_set != other.d->idx_set)
        return false;
    if(d->data2siz != other.d->data2siz)
        return false;

    std::set<size_t>::const_iterator si;
    for(si = d->idx_set.begin(); si != d->idx_set.end(); ++si) {
        if(this->operator [](*si) != other.operator [](*si))
            return false;
    }

    return true;
}

/*! \brief *inequality* relational operator. Returns true if the
 *         *equality* operator CuData::operator == returns false
 *
 * @return true if the *equality* operator CuData::operator==
 *         returns false, false otherwise
 *
 */
bool CuData::operator !=(const CuData &other) const
{
    return !operator ==(other);
}

/** \brief returns true if there are no key-value pairs stored
 *
 * @return true there is no data stored, false if there is at least
 *         one key-value pair
 *
 * @see size
 */
bool CuData::isEmpty() const
{
    return d->idx_set.size() > 0 || (d->datamap && d->datamap->size() > 0);
}

/*! \brief prints the representation of the object provided by toString
 *         using *printf*
 *
 * Prints a string representation of the contents of this object on the
 * console, using printf.
 *
 * @see toString
 */
void CuData::print() const
{
    printf("%s\n", toString().c_str());
}

/*! \brief write a representation of this object into a std::string
 *
 * @return a std::string with a representation of this object.
 *
 * All keys are printed with their values (CuVariant is always
 * converted to string with CuVariant::toString). At last, the
 * number of key/values stored and the flag returned by
 * CuData::isEmpty are printed.
 *
 * @see print
 *
 */
std::string CuData::toString() const
{
    CuXDTypeUtils dt;
    std::string r = "CuData  ";
    std::map<std::string, CuVariant>::const_iterator i;
    if(d->datamap) {
        if(d->datamap->size() > 0)
            r += "*string-keys* { ";
        for(i = d->datamap->begin(); i != d->datamap->end(); ++i)
        {
            r += "[\"" + i->first + "\" -> \"" + i->second.toString() + "\"], ";
        }
        r.replace(r.length() - 2, 2, "");
        r += " } (str map size: "  + std::to_string(stringmapsize()) + ") ";
    }

    std::set<size_t>::const_iterator si;
    std::map<std::string, std::string> valmap; // want a lexicographically ordered print of key name/values
    for(si = d->idx_set.begin(); si != d->idx_set.end(); ++si) {
        valmap[dt.keyName(static_cast<CuDType::Key>(*si))] = operator [](*si).toString();
    }
    r += ("*int-keys* { ");
    for(std::map<std::string, std::string>::const_iterator it = valmap.begin(); it != valmap.end(); ++it) {
        r += "[" + it->first + ": \"" + it->second + "\"], ";
    }
    r.replace(r.length() - 2, 2, "");

    r += " } (int key count: " + std::to_string(d->idx_set.size()) + " isEmpty: " + std::to_string(isEmpty()) +
             " total size: " + std::to_string(d->idx_set.size() + stringmapsize()) + ")";

    return r;
}

/*! \brief put date and time information into this object
 *
 * writes the following keys into this object, taking the system clock time from the
 * gettimeofday call.
 *
 * \li "timestamp_ms" timestamp in milliseconds, convert with CuVariant::toLongInt
 * \li "timestamp_us" timestamp in microseconds, convert with CuVariant::toLongInt
 */
void CuData::putTimestamp()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    add(CuDType::Time_ms,  tv.tv_sec * 1000 + tv.tv_usec / 1000);
    add(CuDType::Time_us, static_cast<double>(tv.tv_sec) + static_cast<double>(tv.tv_usec) * 1e-6);
}

void CuData::mCopyData(const CuData& other)
{
//    pyellowtmp("mCopyData: %s", other.toString().c_str());
    std::map<std::string, CuVariant>::const_iterator it;
    if(d->datamap)
        d->datamap->clear();
    if(other.d->datamap) {
        for (it = other.d->datamap->begin(); it != other.d->datamap->end(); ++it)
            add(it->first, it->second);
    }
    for(std::set<size_t>::const_iterator it = other.d->idx_set.begin(); it != other.d->idx_set.end(); ++it) {
        add(*it, other.operator [](*it));
    }
}

// insert data into d->data2 if the index is beyond CuDType::MaxDataKey
// (re)alloc d->data2 if necessary
//
void CuData::m_insertDataExtra(int index, const CuVariant &v)
{
    int i_off = index - CuDType::MaxDataKey;
    if(i_off < 0)
        return;
    m_reallocData2(i_off);
    pgreen2tmp("CuData::m_insertDataExtra:  finally inserting %s index %d", v.toString().c_str(), index);
    d->data2[i_off] = v;
}

// reallocs d->data2 so that its new size (d->data2siz) is greater than or equal to index
// copies old data into d->data2
//
void CuData::m_reallocData2(int index) {
    if(index < 0)
        return;

    size_t oldsiz = d->data2siz;
    while(d->data2siz <= static_cast<size_t>(index)) {
        d->data2siz += DATA2CHUNK;
        pgreen2tmp("CuData::m_insertDataExtra: data2 will increase to %ld", d->data2siz);
    }
    if(d->data2siz != oldsiz) {
        pgreen2tmp("CuData::m_insertDataExtra: resizing new data2 container to %ld and copying old data (size %ld)",
                   d->data2siz, oldsiz);
        CuVariant *new_data = new CuVariant[d->data2siz];
        // copy old data. If data2 is NULL, then oldsiz is 0
        for(size_t i = 0; i < oldsiz; i++)
            new_data[i] = d->data2[i];

        if(d->data2) {
            printf("\e[1;31mdeleting old d->data2\e[0m\n");
            delete [] d->data2;
        }
        d->data2 = new_data;
    }
}


