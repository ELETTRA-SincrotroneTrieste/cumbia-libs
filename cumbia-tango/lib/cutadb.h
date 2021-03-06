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

    CuTaDb(const TSource& src, CumbiaTango *ct);
    virtual ~CuTaDb();

    void setOptions(const CuData& options);

    // CuThreadListener interface
public:
    void onProgress(int, int, const CuData &);
    void onResult(const CuData &data);
    void onResult(const std::vector<CuData> &datalist);

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
    bool exiting() const;

private:
    CuTaDbPrivate *d;
};

#endif // CUTADB_H
