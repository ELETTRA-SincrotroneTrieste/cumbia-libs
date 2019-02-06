#ifndef MSGFORMATTER_H
#define MSGFORMATTER_H

class QDateTime;
class CuData;

#include <QString>
#include "historyentry.h"

class MsgFormatter
{
public:
    enum FormatOption { Short, Medium, Long, AllCuDataKeys, MaxOption = 32 };

    MsgFormatter();

    QString lastOperation(const QDateTime& dt, const QString& name) const;

    QString history(const QList<HistoryEntry> &hel) const;

    QString fromData(const CuData& d, FormatOption f = Short);

    QString error(const QString& origin, const QString& message);

    QString qualityString() const;  // available if fromData is called

    QString source() const;  // available if fromData is called

    QString value() const;  // available if fromData is called

    QString formulaChanged(const QString &src, const QString &old, const QString &new_f);

    QString hostChanged(const QString& host, bool success) const;

    QString host(const QString& host) const;


private:
    QString m_quality, m_value, m_src;
};

#endif // MSGFORMATTER_H
