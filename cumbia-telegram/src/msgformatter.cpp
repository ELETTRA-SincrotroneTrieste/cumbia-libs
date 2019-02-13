#include "msgformatter.h"
#include "formulahelper.h"
#include "botconfig.h"
#include <QDateTime>
#include <cudata.h>
#include <QtDebug>

#include <cutango-world.h>
#include <cutango-world-config.h>

MsgFormatter::MsgFormatter()
{

}

QString MsgFormatter::lastOperation(const QDateTime &dt, const QString &name) const
{
    if(name.isEmpty())
        return "You haven't performed any operation yet!";
    QString msg = "<i>" + m_timeRepr(dt) + "</i>\n";
    msg += "operation: <b>" + name + "</b>";
    return msg;
}

QString MsgFormatter::history(const QList<HistoryEntry> &hel, int ttl, const QString& type) const
{
    QString msg;
    if(hel.size() == 0)
        msg = type + " list is empty";
    else {
        // heading
        msg += "<b>" + type.toUpper();
        if(type != "bookmarks")
            msg += " HISTORY";
        msg += "</b>\n\n";

        for(int i = 0; i < hel.size(); i++) {
            const HistoryEntry &e = hel[i];
            QString f = e.formula;
            // 1. type + source [+formula if not empty]
            msg += QString::number(i+1) + ". "; // numbered list
            msg += "<i>" + e.name;
            f.isEmpty() ? msg += "</i>" : msg += " " + f.replace(QRegExp("\\s*<\\s+"), " LT ") + "</i>";

            // if bookmark add remove link
            type == "bookmarks" ? msg += QString("   /XB%1\n").arg(e.index) : msg += "\n";

            // 2. if monitor or alert, print stop date and make link to restart if
            //    no more active
            if(e.type == "monitor" || e.type == "alert") {
                QDateTime stop = e.datetime.addSecs(ttl);
                e.is_active ? msg += "<b>active</b> until " : msg += "inactive since ";
                msg += "<i>" + m_timeRepr(stop) + "</i>";

                // stop active monitor by link command
                if(e.is_active && e.index > -1)
                    msg += QString(" stop[/X%1]").arg(e.index);

                msg += "\n";
                if(!e.is_active) {
                    msg += QString("/%1%2 (<i>restart %1").arg(e.type).arg(e.index);
                    e.hasHost() ? msg += "[" + e.host + "]</i>)" : msg += "</i>)";
                }
            }
            else { // one shot read: print date and time
                msg += "<i>" +  m_timeRepr(e.datetime) + "</i>\n";
                msg += "/read" + QString::number(e.index);
                if(!e.host.isEmpty())
                    msg += " (" + e.host + ")";
            }
            msg+="\n";
        }
    }
    printf("\e[1;34m%s\e[0m\n", qstoc(msg));
    return msg;
}

QString MsgFormatter::fromData(const CuData &d, MsgFormatter::FormatOption f)
{
    QString msg, eval_value;
    m_src = QString::fromStdString(d["src"].toString());
    QString host;
    if(m_src.count('/') > 3)
        host = m_src.section('/', 0, 0);
    if(f <= Short && m_src.count('/') > 3) // remove host:PORT/ so that src is not too long
        m_src.replace(0, m_src.indexOf('/', 0) + 1, "");

    long int li = d["timestamp_ms"].toLongInt();
    QDateTime datet = QDateTime::fromMSecsSinceEpoch(li);
    bool ok = !d["err"].toBool();

    // start with timestamp, always
    ok ? msg = "" : msg = "👎";
    msg +=  "<i>" + m_timeRepr(datet) + "</i>";

    !host.isEmpty() ? msg+= " [<i>" + host + "</i>]" : msg += "";

    int idx = d["index"].toInt();

    //  /Xn command used to stop monitor
    if(idx > -1)
        msg += QString("   /X%1").arg(idx);

    msg += "\n"; // new line

    // source: always
    msg += "<i>" + m_src + "</i>: ";

    if(!ok) {
        msg += "\n";
        msg += "😔  <i>" + QString::fromStdString(d["msg"].toString()) + "</i>";
    }
    else { // ok

        if(d.containsKey("formula")) {
            QString formula = QString::fromStdString(d["formula"].toString());
            msg += FormulaHelper().escape(formula) + ": ";
        }
        if(d.containsKey("value"))
            eval_value = m_value = QString::fromStdString(d["value"].toString());

        if(d.containsKey("evaluation"))
            eval_value = QString::fromStdString(d["evaluation"].toString());

        // value
        eval_value.length() < 10 ? msg += " " : msg += "\n";
        msg += "<b>" + eval_value + "</b>";

        // measurement unit if available
        if(d.containsKey("display_unit"))
            msg += " [" + QString::fromStdString(d["display_unit"].toString()) +  "]\n";

        int quality = d["quality"].toInt();
        if(quality != 0) {
            msg += "\n";
            CuTangoWorldConfig twc;
            m_quality = QString::fromStdString(twc.qualityString(static_cast<Tango::AttrQuality>(quality)));
            if(m_quality.compare("ATTR_WARNING", Qt::CaseInsensitive) == 0)
                msg += "😮   <i>warning</i>\n";
            else if(m_quality.compare("ATTR_ALARM", Qt::CaseInsensitive) == 0)
                msg += "😱   <i>alarm</i>\n";
            else if(m_quality.compare("ATTR_INVALID", Qt::CaseInsensitive) == 0)
                msg += "👎   <i>invalid</i>\n";
            else
                msg += "quality: <i>" + m_quality + "</i>\n";
        }

        if(f > Short) {
            msg += "\ndata format: <i>" + QString::fromStdString(d["data_format_str"].toString()) + "</i>\n";
            msg += "\nmode:        <i>" + QString::fromStdString(d["mode"].toString()) + "</i>\n";
        }
    }
    return msg;
}

QString MsgFormatter::error(const QString &origin, const QString &message)
{
    QString msg;
    msg += "👎   " + origin + ": <i>" + message + "</i>";
    return msg;
}

QString MsgFormatter::qualityString() const
{
    return m_quality;
}

QString MsgFormatter::source() const
{
    return m_src;
}

QString MsgFormatter::value() const
{
    return m_value;
}

QString MsgFormatter::formulaChanged(const QString &src, const QString &old, const QString &new_f)
{
    FormulaHelper fh;
    QString s;
    if(!old.isEmpty() && new_f.isEmpty())
        s = "formula <i>" + src + " " + fh.escape(old) + "</i> has been <b>removed</b>";
    else if(old.isEmpty() && new_f.size() > 0)
        s = "formula <b>" + src + " " + fh.escape(new_f)  + "</b> has been introduced";
    else
        s = "formula <i>" + src + " " + fh.escape(old) + "</i>\nchanged into\n<b>" + src + " " + fh.escape(new_f) + "</b>";

    printf("\e[1;33mformulaChanged: %s\e[0m\n", qstoc(s));

    return s;
}

QString MsgFormatter::monitorTypeChanged(const QString &src, const QString &old_t, const QString &new_t)
{
    QString s;
    s = "type <i>" + old_t + "</i> changed into <b>" + new_t + "</b> for source <i>" + src + "</i>";
    return s;
}

QString MsgFormatter::srcMonitorStartError(const QString &src, const QString &message) const
{
    QString s = QString("👎   failed to start monitor for <i>%1</i>:\n"
                        "<b>%2</b>").arg(src).arg(message);
    return s;
}

QString MsgFormatter::monitorUntil(const QString &src, const QDateTime &until) const
{
    return QString("🕐   started monitoring <i>%1</i> until <i>%2</i>").arg(src).arg(m_timeRepr(until));
}

QString MsgFormatter::hostChanged(const QString &host, bool success, const QString &description) const
{
    QString s = "<i>" + m_timeRepr(QDateTime::currentDateTime()) + "</i>\n";
    if(success) {
        s += QString("successfully set host to <b>%1</b>:\n<i>%2</i>").arg(host).arg(description);
    }
    else {
        s += "👎   failed to set host to <b>" + host + "</b>";
    }
    return s;
}

QString MsgFormatter::host(const QString &host) const
{
    QString s;
    s = "host is set to <b>" + host + "</b>:\n";
    s += "It can be changed with:\n"
         "<i>host tango-host:PORT_NUMBER</i>";
    return s;
}

QString MsgFormatter::bookmarkAdded(const HistoryEntry &b) const
{
    QString s;
    if(b.isValid()) {
        s += "👍   successfully added bookmark:\n";
        s += QString("<i>%1 %2</i>\ntype: <i>%3</i>  [host: <i>%4</i>]").arg(b.name).arg(b.formula).arg(b.type).arg(b.host);
    }
    else {
        s = "👎   could not add the requested bookmark";
    }
    return s;
}

QString MsgFormatter::bookmarkRemoved(bool ok) const
{
    QString s;
    ok ? s =  "👍   successfully removed bookmark" : s = "👎   failed to remove bookmark";
    return s;
}

QString MsgFormatter::tg_devSearchList(const QStringList &devs) const
{
    QString s;
    if(devs.isEmpty())
        s = "😞   No device found matching the given pattern";
    else {
        s = QString("<b>%1 DEVICES</b>\n\n").arg(devs.size());
        for(int i = 0; i < devs.size(); i++) {
            s += QString("%1: <i>" + devs[i] + "</i>   [/attlist%1]\n").arg(i+1);
        }
    }
    return s;
}

QString MsgFormatter::tg_attSearchList(const QString& devname, const QStringList &atts) const
{
    QString s;
    if(atts.isEmpty())
        s = "😞   No attributes found within the device <i>" + devname + "</i>\n";
    else {
        s = QString("<b>%1 ATTRIBUTES</b> from <i>%2</i>\n\n").arg(atts.size()).arg(devname);
        for(int i = 0; i < atts.size(); i++) {
            s += QString("%1: <i>" + atts[i] + "</i>   [/a%1_read]\n").arg(i+1);
        }
    }
    return s;
}

QString MsgFormatter::errorVolatileSequence(const QStringList &seq) const
{
    QString s = "😞   The commands <i> ";
    for(int i = 0; i < seq.size() -1; i++) {
        s += seq[i] + ", ";
    }
    if(seq.size() > 0)
        s += seq.last();

    s +=  " </i>\nmust be executed in sequence";
    return s;
}

QString MsgFormatter::volatileOpExpired(const QString &opnam, const QString &text) const
{
    QString s;
    s += QString("⌛️   info: data for the operation \"%1\" has been cleared\n"
                 "Please execute <i>%2</i> again if needed").arg(opnam).arg(text);
    return s;
}

QString MsgFormatter::unauthorized(const QString &username, const char *op_type, const QString &reason) const
{
    QString s = QString("❌   user <i>%1</i> (%2) <b>unauthorized</b>:\n<i>%3</i>")
            .arg(username).arg(op_type).arg(reason);

    return s;
}

QString MsgFormatter::m_timeRepr(const QDateTime &dt) const
{
    QString tr; // time repr
    QTime midnight = QTime(0, 0);
    QDateTime today_midnite = QDateTime::currentDateTime();
    today_midnite.setTime(midnight);
    QDateTime yesterday_midnite = today_midnite.addDays(-1);
    QString date, time = dt.time().toString("hh:mm:ss");

    if(dt >= today_midnite)
        date = "today";
    else if(dt >= yesterday_midnite)
        date = "yesterday";
    else
        date = dt.date().toString("yyyy.MM.dd");
    tr = date + " " + time;
    return tr;
}
