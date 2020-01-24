#ifndef QUR_HDBHELPER_H
#define QUR_HDBHELPER_H

#include <cudata.h>
#include <QString>
#include <QMap>

class QuR_HdbHelper
{
public:
    QuR_HdbHelper();

    void mergeResult(const QString& src, const CuData& data);
    bool isComplete(const QString &name) const;
    bool allComplete() const;
    CuData take(const QString& src);
    QList<CuData> takeAll();

    void print_all(const QList<CuData> & dl, const QString& out_filenam = QString(), bool color = true);

private:
    QMap<QString, CuData> mMap;
};

#endif // QUR_HDBHELPER_H
