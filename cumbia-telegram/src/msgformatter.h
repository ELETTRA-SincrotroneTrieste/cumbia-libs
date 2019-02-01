#ifndef MSGFORMATTER_H
#define MSGFORMATTER_H

class QDateTime;
class CuData;

#include <QString>


class MsgFormatter
{
public:
    enum FormatOption { Short, Medium, Long, AllCuDataKeys, MaxOption = 32 };

    MsgFormatter();

    QString lastOperation(const QDateTime& dt, const QString& name) const;

    QString history(const QStringList& cmd_shortcuts, const QStringList& timestamps, const QStringList& cmds) const;

    QString fromData(const CuData& d, FormatOption f = Short);
};

#endif // MSGFORMATTER_H
