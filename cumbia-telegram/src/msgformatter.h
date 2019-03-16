#ifndef MSGFORMATTER_H
#define MSGFORMATTER_H

class QDateTime;
class CuData;
class CuVariant;

#include <QString>
#include "historyentry.h"
#include "aliasentry.h"
#include "tbotmsgdecoder.h" // for Type
#include "../cumbia-telegram-defs.h"

class MsgFormatter
{
public:
    enum FormatOption { Short, Medium, Long, AllCuDataKeys, MaxOption = 32 };

    MsgFormatter();

    QString lastOperation(const QDateTime& dt, const QString& name) const;

    QString history(const QList<HistoryEntry> &hel, int ttl, const QString &type) const;

    QString fromData(const CuData& d, FormatOption f = Short);

    QString error(const QString& origin, const QString& message);

    QString qualityString() const;  // available if fromData is called

    QString source() const;  // available if fromData is called

    QString value() const;  // available if fromData is called

    QString formulaChanged(const QString &src, const QString &old, const QString &new_f);
    QString monitorTypeChanged(const QString &src, const QString &old_t, const QString &new_t);
    QString srcMonitorStartError(const QString &src, const QString &message) const;
    QString monitorUntil(const QString& src, const QDateTime& until) const;
    QString monitorStopped(const QString& cmd, const QString& msg) const;


    QString hostChanged(const QString& host, bool success, const QString& description) const;

    QString host(const QString& host) const;

    QString bookmarkAdded(const HistoryEntry& b) const;
    QString bookmarkRemoved(bool ok) const;

    QString tg_devSearchList(const QStringList& devs) const;
    QString tg_attSearchList(const QString &devname, const QStringList& devs) const;

    QString errorVolatileSequence(const QStringList &seq) const;
    QString volatileOpExpired(const QString &opnam, const QString &text) const;

    QString unauthorized(const QString& username, const char* op_type, const QString& reason) const;

    QString fromControlData(const ControlMsg::Type t, const QString& msg) const;

    QString help(TBotMsgDecoder::Type t) const;

    QString aliasInsert(bool success, const QStringList &alias_parts, const QString& additional_message) const;

    QString aliasList(const QString &name, const QList<AliasEntry>& alist) const;

    QString botShutdown();

private:
    QString m_quality, m_value, m_src;

    QString m_timeRepr(const QDateTime& dt) const;

    QString m_getVectorInfo(const CuVariant& v);

    void m_cleanSource(const QString& s, QString &point, QString &device, QString& host, MsgFormatter::FormatOption f) const;
};

#endif // MSGFORMATTER_H
