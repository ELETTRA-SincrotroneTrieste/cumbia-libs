#include "historyentry.h"

HistoryEntry::HistoryEntry(int idx, int u_id, const QDateTime &ts,
                           const QString &nam, const QString &ty,
                           const QString &f, const QString &_host)
{
    index = idx;
    user_id = u_id;
    datetime = ts;
    name = nam;
    type = ty;
    formula = f;
    host = _host;
    is_active = false;
    chat_id = -1;
}

HistoryEntry::HistoryEntry(int u_id, const QString &nam, const QString &typ,
                           const QString &formu, const QString &_host)
{
    user_id = u_id;
    name = nam;
    type = typ;
    formula = formu;
    host = _host;
    is_active = false;
    datetime = QDateTime::currentDateTime();
    chat_id = -1;
}

HistoryEntry::HistoryEntry()
{
    index = -1;
    user_id = -1;
    is_active = false;
}

bool HistoryEntry::isValid() const
{
    return user_id > -1 || datetime.isValid() || !name.isEmpty();
}

bool HistoryEntry::hasChatId() const {
    return chat_id > -1;
}

// user_id,chat_id,name,type,formula,host,start_dt
void HistoryEntry::fromDbProc(int u_id, int chatid, const QString& nam, const QString& typ,
                              const QString& formul, const QString& hos,
                              const QDateTime& dt)
{
    user_id = u_id;
    chat_id = chatid;
    name = nam;
    type = typ;
    formula = formul;
    host = hos;
    datetime = dt;
    index = -1;
}

QString HistoryEntry::toCommand() const
{
    QString s;
    if(user_id > -1) {
        if(type == "monitor" || type == "alert")
            s = type + " ";
        s += name + " " + formula;
    }
    return s;
}

bool HistoryEntry::hasHost() const
{
    return !host.isEmpty();
}

bool HistoryEntry::hasFormula() const
{
    return !formula.isEmpty();
}
