#include "cudata.h"
#include "cumacros.h"
#include <string>
#include <string.h>
#include <sys/time.h>
#include <unordered_map>
#include <map>
#include <atomic>

/*! @private */
class CuDataPrivate
{
public:
    CuDataPrivate(const CuDataPrivate& other) {
        datamap = other.datamap;
        _r.store(1);
    }
    CuDataPrivate() {
        _r.store(1);
    }
    ~CuDataPrivate() {
    }

    std::map<std::string, CuVariant> datamap;
    CuVariant emptyVariant;

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

/*! \brief the class destructor
 *
 * CuData class destructor
 */
CuData::~CuData() {
    if(d_p && d_p->unref() == 1)
        delete d_p;

}

/*! \brief constructor of an empty CuData
 *
 * builds a CuData with no key/value
 */
CuData::CuData() {
    d_p = new CuDataPrivate();
}

/*! \brief constructor of a CuData initialised with one value (key: emtpy string)
 *
 * @param v a CuVariant used to initialise a key/value pair where the key
 *        is an empty string.
 *
 * The parameter-less of CuData::value can be used to get the value associated
 * to an empty key
 */
CuData::CuData(const CuVariant &v) {
    d_p = new CuDataPrivate();
    d_p->datamap[""] = v;
}

/*! \brief constructor initialising a CuData with one key-value pair
 *
 * @param key a string used as a key
 * @param v the value, as CuVariant
 *
 * The new CuData will be initialised with a key/v pair
 */
CuData::CuData(const std::string& key, const CuVariant &v) {
    d_p = new CuDataPrivate();
    d_p->datamap[key] = v;
}

/*! \brief the copy constructor
 *
 * @param other another CuData used as the source of the copy
 *
 * copies all the contents from the *other* CuData into the
 * new object
 */
CuData::CuData(const CuData &other) {
    d_p = other.d_p;
    d_p->ref(); // increment ref counter
    //    mCopyData(other); // before implicit sharing
}

/*! \brief c++11 *move constructor*
 *
 * @param other another CuData used as the source of the move operation
 *
 * Contents of *other* are moved into *this* CuData
 */
CuData::CuData(CuData &&other) {
    d_p = other.d_p; /* no d = new here */
    other.d_p = nullptr; /* avoid deletion! */
}

/*! \brief assignment operator, copies data from another source
 *
 * @param other another CuData which values will be copied into this
 */
CuData &CuData::operator=(const CuData &other) {
    if(this != &other) {
        other.d_p->ref();
        if(d_p->unref() == 1)
            delete d_p; // with no sharing we would not delete

        d_p = other.d_p;
        //        mCopyData(other); // before implicit sharing
    }
    return *this;
}

/*! \brief move assignment operator, moves contents from another source
 *
 * @param other another CuData which values will be moved into this
 */
CuData &CuData::operator=(CuData &&other) {
    if (this!=&other) {
        if(d_p && d_p->unref() == 1)
             delete d_p;
        d_p = other.d_p;
        other.d_p = nullptr; /* avoid deletion! */
    }
    return *this;
}

CuData &CuData::set(const std::string &key, const CuVariant &value) {
    detach();
    d_p->datamap[key] = value;
    return *this;
}

CuData &CuData::merge(const CuData &&other) {
    detach();
    for(const std::string& key : other.keys())
        (*this).set(key, std::move(other.value(key)));
    return *this;
}

/*! \brief clone this data, without sharing
 *
 *  \return a clone of this data, bypassing the copy-on-write share
 */
CuData CuData::clone() const {
    CuData d;
    d.d_p = new CuDataPrivate(*d_p);
    return d;
}

CuData &CuData::merge(const CuData &other) {
    detach();
    for(const std::string& key : other.keys())
        (*this).set(key, other.value(key));
    return *this;
}

CuData &CuData::remove(const std::string &key) {
    detach();
    d_p->datamap.erase(key);
    return *this;
}

CuData CuData::remove(const std::string &key) const {
    return CuData(*this).remove(key);
}

CuData &CuData::remove(const std::vector<std::string> &keys) {
    detach();
    for(const std::string& k : keys)
        d_p->datamap.erase(k);
    return *this;
}

CuData CuData::remove(const std::vector<std::string> &keys) const {
    return CuData(*this).remove(keys);
}

/** \brief returns the number of key-value pairs stored
 *
 * @return the number of key-value pairs stored within this object
 *
 * @see isEmpty
 */
size_t CuData::size() const
{
    return d_p->datamap.size();
}

/*! \brief return the value associated to the empty key
 *
 * @return the value associated to an empty key, if there's one empty key
 *         with a value (for example, created with the CuData::CuData(const CuVariant &v) )
 *         constructor, or an *empty* CuVariant
 *
 * If there's no value associated to an *empty key* (i.e. an *emtpy string*, ""), an
 * *invalid and null* CuVariant is returned. See CuVariant::isValid and CuVariant::isNull
 */
CuVariant CuData::value() const {
    /* search the empty key */
    if(d_p->datamap.count("") > 0)
        return d_p->datamap.at("");
    return CuVariant();
}

/*! \brief return a copy of the value associated with the given *key*,
 *         or an *invalid and null* CuVariant
 *
 * @return a copy of the value associated with the given *key*,
 *         or an *invalid and null* CuVariant. See CuVariant::isValid and
 *         CuVariant::isNull
 *
 * See also CuData::operator [](const std::string &key) const
 */
CuVariant CuData::value(const std::string & key) const {
    if(d_p->datamap.count(key) > 0)
        return d_p->datamap[key];
    return CuVariant();
}

/*! \brief insert the key/value into the bundle
 *
 * @param key the new key, as a std::string
 * @param value the new value bound to *key*
 *
 * \note The effect is exactly the same as using the operator [] (const std::string& key)
 */
void CuData::add(const std::string & key, const CuVariant &value) {
    detach();
    d_p->datamap[key] = value;
}

/*! \brief returns true if the bundle contains the given key
 *
 * @param key the key to be searched, std::string
 * @return true if the bundle contains the given key, false otherwise
 */
bool CuData::containsKey(const std::string &key) const
{
    return d_p->datamap.count(key) > 0;
}

/*! \brief returns true if the specified key has the given string value
 *
 * This method is a shortcut to test if to a given key is associated a string with the given value.
 *
 * @param key the key
 * @param value the value *as string*
 * @return true if *data[key].toString() == value* false otherwise
 */
bool CuData::has(const std::string &key, const std::string &value) const
{
    bool ok;
    return d_p->datamap.count(key) > 0 && d_p->datamap.at(key).toString(&ok) == value && ok;
}

/*! \brief array subscript write operator; pushes a new key-value pair into the bundle
 *
 * Use the insert operator to add a new key/value pair to the bundle
 *
 * \par  Example
 * \code
    CuData at("src", source.getName()); // string
    at["device"] = source.getDeviceName(); // string
    at["activity"] = "E"; // string
    at["period"] = source.period(); // integer
    at["err"] = false; // bool
 * \endcode
 */
CuVariant &CuData::operator [](const std::string &key) {
    detach();
    return d_p->datamap[key];
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
    if(d_p->datamap.count(key) > 0)
        return d_p->datamap[key];
    return d_p->emptyVariant;
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
    if(other.d_p->datamap.size() != d_p->datamap.size())
        return false;
    std::map<std::string, CuVariant>::const_iterator i;
    for(i = d_p->datamap.begin(); i != d_p->datamap.end(); ++i)
    {
        if(!other.containsKey(i->first))
            return false;
        if(other[i->first] != i->second)
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
    return d_p->datamap.size() == 0;
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
    std::string r = "CuData { ";
    std::map<std::string, CuVariant>::const_iterator i;
    char siz[16], empty[16];
    snprintf(siz, 16, "%ld", size());
    snprintf(empty, 16, "%d", isEmpty());
    for(i = d_p->datamap.begin(); i != d_p->datamap.end(); ++i)
    {
        r += "[\"" + i->first + "\" -> " + i->second.toString() + "], ";
    }
    r.replace(r.length() - 2, 2, "");
    r += " } (size: " + std::string(siz) + " isEmpty: " + std::string(empty) + ")";
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
void CuData::putTimestamp() {
    detach();
    struct timeval tv;
    gettimeofday(&tv, NULL);
    add("timestamp_ms",  tv.tv_sec * 1000 + tv.tv_usec / 1000);
    add("timestamp_us", static_cast<double>(tv.tv_sec) + static_cast<double>(tv.tv_usec) * 1e-6);
}

std::vector<std::string> CuData::keys() const
{
    std::vector<std::string> ks;
    for(std::map<std::string, CuVariant>::const_iterator it = d_p->datamap.begin(); it != d_p->datamap.end(); ++it)
        ks.push_back(it->first);
    return ks;
}

// this was used before implicit sharing
//void CuData::mCopyData(const CuData& other)
//{
//    std::map<std::string, CuVariant>::const_iterator it;
//    for (it = other.d_p->datamap.begin(); it != other.d_p->datamap.end(); ++it)
//        d_p->datamap[it->first] = other.d_p->datamap[it->first];
//}

void CuData::detach() {
    if(d_p->load() > 1) {
        d_p->unref();
        d_p = new CuDataPrivate(*d_p); // sets ref=1
    }
}

std::string CuData::s(const std::string& key) const {
    return this->operator[](key).s();
}

double CuData::d(const std::string& key) const {
    return this->operator[](key).d();
}

int CuData::i(const std::string& key) const {
    return this->operator[](key).i();
}

unsigned int CuData::u(const std::string &key) const {
    return this->operator[](key).u();
}

bool CuData::b(const std::string& key) const {
    if(containsKey(key))
        return this->operator[](key).d();
    return false;
}

// to<T> version shortcuts


double CuData::D(const std::string& key) const {
    double v = 0.0;
    if(containsKey(key))
        this->operator[](key).to<double>(v);
    return v;
}

int CuData::I(const std::string& key) const {
    int i = 0.0;
    if(containsKey(key))
        this->operator[](key).to<int>(i);
    return i;
}

unsigned int CuData::U(const std::string &key) const {
    unsigned int i = 0.0;
    if(containsKey(key))
        this->operator[](key).to<unsigned int>(i);
    return i;
}

bool CuData::B(const std::string& key) const {
    bool b = false;
    if(containsKey(key))
        this->operator[](key).to<bool>(b);
    return b;
}

std::vector<double> CuData::DV(const std::string &key) const {
    std::vector<double>  dv;
    if(containsKey(key))
        this->operator[](key).toVector<double>(dv);
    return dv;
}

std::vector<int>  CuData::IV(const std::string &key) const {
    std::vector<int>  vi;
    if(containsKey(key))
        this->operator[](key).toVector<int>(vi);
    return vi;
}

std::vector<long long> CuData::LLV(const std::string &key) const {
    std::vector< long long int>  lliv;
    if(containsKey(key))
        this->operator[](key).toVector< long long int>(lliv);
    return lliv;
}

std::vector<unsigned int>  CuData::UV(const std::string &key) const {
    std::vector<unsigned int>  uiv;
    if(containsKey(key))
        this->operator[](key).toVector<unsigned int>(uiv);
    return uiv;
}

std::vector<unsigned long> CuData::ULV(const std::string &key) const {
    std::vector<unsigned long int>  uliv;
    if(containsKey(key))
        this->operator[](key).toVector<unsigned long int>(uliv);
    return uliv;
}

std::vector<unsigned long long> CuData::ULLV(const std::string &key) const {
    std::vector<unsigned long long int>  ulliv;
    if(containsKey(key))
        this->operator[](key).toVector<unsigned long long int>(ulliv);
    return ulliv;
}

std::vector<bool> CuData::BV(const std::string &key) const {
    std::vector<bool>  bv;
    if(containsKey(key))
        this->operator[](key).toVector<bool>(bv);
    return bv;
}

