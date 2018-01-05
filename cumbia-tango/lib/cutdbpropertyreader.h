#ifndef CUTDBPROPERTYREADER_H
#define CUTDBPROPERTYREADER_H

#include <cuthreadlistener.h>
#include <list>

class CuTDbPropertyReaderPrivate;
class CumbiaTango;
class CuDataListener;

class CuTDbPropertyReader : public CuThreadListener
{
public:
    CuTDbPropertyReader(const std::string& id, CumbiaTango *cumbia_t);

    virtual ~CuTDbPropertyReader();

    void get(const std::list<CuData>& in_data);

    void addListener(CuDataListener *l);

    // CuThreadListener interface
public:
    void onProgress(int step, int total, const CuData &data);
    void onResult(const CuData &data);
    CuData getToken() const;


private:
    CuTDbPropertyReaderPrivate *d;
};

#endif // CUTDBPROPERTYREADER_H
