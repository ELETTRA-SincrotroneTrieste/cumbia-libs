#ifndef CUTADB_H
#define CUTADB_H

#include "cutangoactioni.h"

class CuTaDbPrivate;

/*!
 * \brief Get data from the Tango database.
 *
 */
class CuTaDb : public CuTangoActionI
{
public:

    CuTaDb(const TSource& src, CumbiaTango *ct, const CuData& options = CuData(), const CuData& tag = CuData());
    virtual ~CuTaDb();

    void setOptions(const CuData& options);
    void setTag(const CuData& tag);

    // CuThreadListener interface
public:
    void onProgress(int, int, const CuData &);
    void onResult(const CuData &data);
    void onResult(const CuData *p, int siz);

    CuData getToken() const;

    // CuTangoActionI interface
public:
    TSource getSource() const;
    Type getType() const;
    void addDataListener(CuDataListener *l);
    void removeDataListener(CuDataListener *l);
    void sendData(const CuData& data);
    void getData(CuData& d_inout) const;
    size_t dataListenersCount();
    void start();
    void stop();

private:
    CuTaDbPrivate *d;
};

#endif // CUTADB_H
