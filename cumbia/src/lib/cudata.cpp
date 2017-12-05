#include "cudata.h"
#include "cumacros.h"
#include <string>
#include <map>

class CuDataPrivate
{
public:
    std::map<std::string, CuVariant> datamap;

    CuVariant emptyVariant;
};

CuData::~CuData()
{
    if(d)
        delete d;
}

CuData::CuData()
{
    d = new CuDataPrivate();
}

CuData::CuData(const CuVariant &v)
{
    d = new CuDataPrivate();
    d->datamap[""] = v;
}

CuData::CuData(const std::string& key, const CuVariant &v)
{
    d = new CuDataPrivate();
    d->datamap[key] = v;
}

CuData::CuData(const CuData &other)
{
    d = new CuDataPrivate();
    mCopyData(other);
}

CuData::CuData(CuData &&other)
{
    d = other.d; /* no d = new here */
    other.d = NULL; /* avoid deletion! */
}

CuData &CuData::operator=(const CuData &other)
{
    if(this != &other)
        mCopyData(other);
    return *this;
}

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

size_t CuData::size() const
{
    return d->datamap.size();
}

CuVariant CuData::value() const
{
    /* 1. search the empty key */
    if(d->datamap.count("") > 0)
        return d->datamap.at("");
    if(d->datamap.size() > 0)
    {
        std::map<std::string, CuVariant>::const_iterator it = d->datamap.begin();
        return it->second;
    }
    return CuVariant();
}

CuVariant CuData::value(const std::string & key) const
{
    if(d->datamap.count(key) > 0)
        return d->datamap[key];
    return CuVariant();
}

void CuData::add(const std::string & key, const CuVariant &value)
{
    d->datamap[key] = value;
}

bool CuData::containsKey(const std::string &key) const
{
    return d->datamap.count(key) > 0;
}

CuVariant &CuData::operator [](const std::string &key)
{
    return d->datamap[key];
}

const CuVariant &CuData::operator [](const std::string &key) const
{
    d->emptyVariant = CuVariant();
    if(d->datamap.count(key) > 0)
        return d->datamap[key];
    return d->emptyVariant;
}

bool CuData::operator ==(const CuData &other) const
{
    if(other.d->datamap.size() != d->datamap.size())
        return false;
    std::map<std::string, CuVariant>::const_iterator i, j;
    for(i = d->datamap.begin(), j = other.d->datamap.begin(); i != d->datamap.end(); ++i, ++j)
    {
        if(i->first != j->first || i->second != j->second)
            return false;
    }
    return true;
}

bool CuData::operator !=(const CuData &other) const
{
    return !operator ==(other);
}

bool CuData::isEmpty() const
{
    return d->datamap.size() == 0;
}

void CuData::print() const
{
    cuprintf("%s\n", toString().c_str());
}

std::string CuData::toString() const
{
    std::string r = "CuData { ";
    std::map<std::string, CuVariant>::const_iterator i;
    for(i = d->datamap.begin(); i != d->datamap.end(); ++i)
    {
        r += "[\"" + i->first + "\" -> " + i->second.toString() + "], ";
    }
    r.replace(r.length() - 2, 2, "");
    r += " } (size: " + std::to_string(size()) + " isEmpty: " + std::to_string(isEmpty()) + ")";
    return r;
}

void CuData::mCopyData(const CuData& other)
{
    std::map<std::string, CuVariant>::const_iterator it;
    for (it = other.d->datamap.begin(); it != other.d->datamap.end(); ++it)
        d->datamap[it->first] = other.d->datamap[it->first];
}

