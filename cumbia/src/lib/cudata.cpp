#include "cudata.h"
#include "cumacros.h"
#include <string>
#include <string.h>
#include <sys/time.h>
#include <unordered_map>
#include <map>
#include <atomic>

std::atomic<int> dacnt = 0; // data counter
std::atomic<int>  pcnt = 0; // private data counter
std::atomic<int>  wpcnt = 0; // private data counter that would be without sharing
std::atomic<int>  cp = 0; // copy counter
std::atomic<int>  wcp = 0; // would copy without sharing
std::atomic<int>  detachcnt = 0;

/*! @private */
class CuDataPrivate
{
public:
    CuDataPrivate(const CuDataPrivate& other) {
        pcnt++;
        datamap = other.datamap;
        cp++; // TEST: increment copy count
        _r.store(1);
        printf("CuDataPrivate %p \e[1;35;4mHAS MADE A FULL COPY OF A MAP SIZED %ld\e[0m\n"
               "\e[1;35m TOTAL COPIES UNTIL NOW %d WOULD HAVE BEEN %d\e[0m\n",
               this, datamap.size(), cp.load(), wcp.load());
    }

    CuDataPrivate() {
        pcnt++;
        _r.store(1);
    }
    ~CuDataPrivate() {
        pcnt--;
        printf("\e[1;31mx\e[0m ~CuDataPrivate %p ref was %d pcnt is now %d\e[0m\n", this, _r.load(), pcnt.load());
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
    dacnt--;
    if(d_p) {
        int r = d_p->unref();
        if(r == 1) {
            printf("\e[1;31mX ~CuData %p deleting d_p %p cuz refcnt==1 (data cnt %d d_p %d would be %d)\n", this, d_p, dacnt.load(), pcnt.load() -1, wpcnt.load());
            delete d_p;
        }
        else
            printf("\e[1;32mX ~CuData %p not deleting d_p %p cause ref %d\e[0m (data cnt %d d_p %d would be %d)\n", this, d_p, r, dacnt.load(), pcnt.load(), wpcnt.load());
    }
    else
        printf("\e[1;32mX ~CuData %p not deleting d_p cause null, probably \e[1;35mafter a move\e[0m\e[0m (data cnt %d pcnt %d would be %d)\n", this, dacnt.load(), pcnt.load(), wpcnt.load());
    printf("\e[1;31mX\e[0m ~CuData \e[1;36mcopy counter %d would have copied %d - detach() called %d times\e[0m\n", cp.load(), wcp.load(), detachcnt.load());
}

/*! \brief constructor of an empty CuData
 *
 * builds a CuData with no key/value
 */
CuData::CuData() {
    d_p = new CuDataPrivate();
    dacnt++;
    wpcnt++; // there is and there would be a new private data obj
    printf("\e[1;32m+ \e[0m%p d_p %p tot data %d tot d_p %d would be %d\n", this, d_p, dacnt.load(), pcnt.load(), wpcnt.load());
}

/*! \brief constructor of a CuData initialised with one value (key: emtpy string)
 *
 * @param v a CuVariant used to initialise a key/value pair where the key
 *        is an empty string.
 *
 * The parameter-less of CuData::value can be used to get the value associated
 * to an empty key
 */
CuData::CuData(const CuVariant &v)
{
    d_p = new CuDataPrivate();
    d_p->datamap[""] = v;
    dacnt++;
    wpcnt++; // there is and there would be a new private data obj
    printf("\e[1;32m+ \e[0mCuData(const CuVariant &v %s) %p d_p %p tot data %d tot d_p %d would be %d\n", vtoc(v),
           this, d_p, dacnt.load(), pcnt.load(), wpcnt.load());
}

/*! \brief constructor initialising a CuData with one key-value pair
 *
 * @param key a string used as a key
 * @param v the value, as CuVariant
 *
 * The new CuData will be initialised with a key/v pair
 */
CuData::CuData(const std::string& key, const CuVariant &v)
{
    d_p = new CuDataPrivate();
    d_p->datamap[key] = v;
    dacnt++;
    wpcnt++; // there is and there would be a new private data obj
    printf("\e[1;32m+ \e[0mCuData(%s, %s) %p d_p %p tot data %d tot d_p %d would be %d\n",
           key.c_str(), v.toString().c_str(), this, d_p, dacnt.load(), pcnt.load(), wpcnt.load());
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
    dacnt++;
    //    d_p = new CuDataPrivate();
    wpcnt++;

    d_p = other.d_p;
    int  r =  d_p->ref(); // increment ref counter
    printf("\e[1;32mCuData::CuData(const CuData &other) new CuData constr %p reusing d_p %p has refcnt %d\e[0m "
           "tot data %d tot d_p %d would be %d \e[1;36mcopy counter %d would have copied %d \e[1;3;32;4mSAVED ONE COPY\e[0m\n",
           this, d_p, r, dacnt.load(), pcnt.load(), wpcnt.load(), cp.load(), wcp.load());
    //    mCopyData(other);
    wcp++;
    //  ^^ would have copied
}

/*! \brief c++11 *move constructor*
 *
 * @param other another CuData used as the source of the move operation
 *
 * Contents of *other* are moved into *this* CuData
 */
CuData::CuData(CuData &&other)
{
    d_p = other.d_p; /* no d = new here */
    int r = d_p->load();
    dacnt++;
    printf("\e[1;32mCuData::CuData [MOVE] this %p reusing d_p %p (other.d_p %p --> null) has refcnt %d\e[0m tot data %d tot d_p %d would be %d\n",
           this, d_p, other.d_p, r, dacnt.load(), pcnt.load(), wpcnt.load());
    other.d_p = nullptr; /* avoid deletion! */
}

/*! \brief assignment operator, copies data from another source
 *
 * @param other another CuData which values will be copied into this
 */
CuData &CuData::operator=(const CuData &other)
{
    if(this != &other) {
        other.d_p->ref();
        int r = this->d_p->unref();
        if(r == 1) {
            // without sharing we would NOT delete
            printf("\e[1;35mCuData::operator= CuData %p assignment from other %p deletes reference %p\e[0m\n",
                   this, &other, d_p);
            delete d_p;
        }
        d_p = other.d_p;

        //        d_p->datamap.clear();
        //        mCopyData(other);
        wcp++;
        // ^^ // would have copied

        printf("\e[1;32mCuData::operator= %p reusing d_p %p has refcnt %d\e[0m "
               "tot data %d tot d_p %d would be %d \e[1;36mcopy counter %d would have copied %d \e[1;3;32;4mSAVED ONE COPY\e[0m\n",
               this, d_p, d_p->load(), dacnt.load(), pcnt.load(), wpcnt.load(), cp.load(), wcp.load());
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
        if(d_p) {
            int r = d_p->unref();
            if(r == 1) {
                printf("CuData::operator= [\e[1;35mMOVE\e[0m] this %p deleting d_p %p\n", this, d_p);
                delete d_p;
            }
        }
        d_p = other.d_p;

        printf("\e[1;32mCuData::operator= [MOVE ASSIGNMENT] this %p d_p %p (other.d_p %p will be set to null in order not to delete) has refcnt %d\e[0m tot data %d tot d_p %d would be %d\n",
               this, d_p, other.d_p, d_p->load(), dacnt.load(), pcnt.load(), wpcnt.load());

        other.d_p = nullptr; /* avoid deletion! */

    }
    return *this;
}

CuData &CuData::set(const std::string &key, const CuVariant &value) {
    detach();
    d_p->datamap[key] = value;
    printf("\e[1;33m$$\e[0m CuData::set this %p key %s val %s \e[1;36mcopy counter %d would have copied %d\e[0m\n",
           this, key.c_str(), value.toString().c_str(), cp.load(), wcp.load());
    return *this;
}

CuData &CuData::merge(const CuData &&other) {
    detach();
    for(const std::string& key : other.keys())
        (*this).set(key, std::move(other.value(key)));
    printf("\e[1;33m$$\e[0m CuData::merge this %p  \e[1;36mcopy counter %d would have copied %d\e[0m\n", this, cp.load(), wcp.load());
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
CuVariant CuData::value(const std::string & key) const
{
    printf("CuData value() const this %p d_p %p key %s\n", this, d_p, key.c_str());
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
void CuData::add(const std::string & key, const CuVariant &value)
{
    detach();
    printf("\e[1;33m$$\e[0m CuData::add this %p key %s val %s  \e[1;36mcopy counter %d would have copied %d\e[0m\n",
           this, key.c_str(), value.toString().c_str(), cp.load(), wcp.load());
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
    printf("\e[1;33m$$\e[0m CuData::operator [] (non const) this %p key %s\n", this, key.c_str());
    detach();
    printf("\e[1;33m$$\e[0m CuData::add this %p key %s  \e[1;36mcopy counter %d would have copied %d\e[0m\n",
           this, key.c_str(), cp.load(), wcp.load());
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
void CuData::putTimestamp()
{
    printf("\e[1;33m$$\e[0m CuData::putTimestamp this %p\n", this);
    detach();
    struct timeval tv;
    gettimeofday(&tv, NULL);
    add("timestamp_ms",  tv.tv_sec * 1000 + tv.tv_usec / 1000);
    add("timestamp_us", static_cast<double>(tv.tv_sec) + static_cast<double>(tv.tv_usec) * 1e-6);
    printf("\e[1;33m$$\e[0m CuData::putTimestamp this %p \e[1;36mcopy counter %d would have copied %d\e[0m\n",
           this,  cp.load(), wcp.load());
}

std::vector<std::string> CuData::keys() const
{
    std::vector<std::string> ks;
    for(std::map<std::string, CuVariant>::const_iterator it = d_p->datamap.begin(); it != d_p->datamap.end(); ++it)
        ks.push_back(it->first);
    return ks;
}

//void CuData::mCopyData(const CuData& other)
//{
//    std::map<std::string, CuVariant>::const_iterator it;
//    for (it = other.d_p->datamap.begin(); it != other.d_p->datamap.end(); ++it)
//        d_p->datamap[it->first] = other.d_p->datamap[it->first];
//}

void CuData::detach() {
    detachcnt++;
    int r = d_p->load();
    if(r > 1) {
        printf("CuData::\e[0;32mdetach:\e[0m: this %p old_dp %p need new d_p because cur ref %d\n", this, d_p, r);
        d_p->unref();
        d_p = new CuDataPrivate(*d_p); // sets ref=1
        // in test, new CuDataPrivate increments cp copy counter. In fact the constructor MAKES a COPY
        printf(" -> new d_p %p \e[1;36mcopy counter %d would have copied %d detach call count %d\e[0m\n", d_p, cp.load(), wcp.load(), detachcnt.load());
    }
    else {
        printf("CuData::\e[0;35mdetach:\e[0m: this %p do not need new d_p (remains %p) because cur "
               "ref %d\e[1;36mcopy counter %d would have copied %d detach() called %d times\e[0m\n",
               this, d_p, r, cp.load(), wcp.load(), detachcnt.load());
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

