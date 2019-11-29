#ifndef TGDBPROPHELPER_H
#define TGDBPROPHELPER_H

#ifdef QUMBIA_TANGO_CONTROLS_VERSION

#include <QStringList>

class CumbiaPool;
class CuDataListener;
class CuTDbPropertyReader;

class TgDbPropHelper {
public:
    TgDbPropHelper(CuDataListener *li);
    ~TgDbPropHelper();

    void get(CumbiaPool *cu_p, const QStringList &proplist);

private:
    CuDataListener *m_listener;
    CuTDbPropertyReader *m_dbr;
};

#endif // QUMBIA_TANGO_CONTROLS_VERSION

#endif // TGDBPROPHELPER_H
