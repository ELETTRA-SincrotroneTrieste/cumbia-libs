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
    QString msg = "<i>" + dt.toString() + "</i>\n";
    msg += "operation: <b>" + name + "</b>";
    return msg;
}

QString MsgFormatter::history(const QList<HistoryEntry> &hel) const
{
    QString msg;
    if(hel.size() == 0)
        msg = "history empty!";
    else {
        for(int i = 0; i < hel.size(); i++) {
            const HistoryEntry &e = hel[i];
            QString f = e.formula;
            // 1. type + source [+formula if not empty]
            msg +=  "<b>"+ e.type + "</b>: <i>" + e.name;
            f.isEmpty() ? msg += "</i>\n" : msg += " " + f.replace(QRegExp("\\s*<\\s+"), " LT ") + "</i>\n";

            // 2. if monitor or alert, print stop date and make link to restart if
            //    no more active
            if(e.type == "monitor" || e.type == "alert") {
                QDateTime stop = e.datetime.addSecs(BotConfig().ttl());
                e.is_active ? msg += "<b>active</b> until " : msg += "inactive since ";
                msg += "<i>" + stop.toString("yyyy.MM.dd hh.mm.ss") + "</i>\n";
                if(!e.is_active) {
                    msg += QString("/%1%2 (<i>restart %1").arg(e.type).arg(e.index);
                    e.hasHost() ? msg += "[" + e.host + "]</i>)" : msg += "</i>)";
                }
            }
            else { // one shot read: print date and time
                msg += "<i>" + e.datetime.toString("yyyy.MM.dd hh.mm.ss") + "</i>\n";
                msg += "/read" + QString::number(e.index);
                if(!e.host.isEmpty())
                    msg += " (" + e.host + ")";
            }
            msg+="\n\n";
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
    QString ts = QDateTime::fromMSecsSinceEpoch(li).toString("yyyy.MM.dd hh:mm:ss");
    bool ok = !d["err"].toBool();

    // start with timestamp, always
    ok ? msg = "" : msg = "👎";
    msg +=  "<i>" +ts + "</i>";
    !host.isEmpty() ? msg+= " [<i>" + host + "</i>]\n" : msg += "\n";
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
        s = "formula <b>" + src + " " + fh.escape(old) + "</i> has been removed";
    else if(old.isEmpty() && new_f.size() > 0)
        s = "formula <b>" + src + " " + fh.escape(new_f)  + "</b> has been introduced";
    else
        s = "formula <i>" + src + " " + fh.escape(old) + "</i>\nchanged into\n<b>" + src + " " + fh.escape(new_f) + "</b>";
    return s;
}

QString MsgFormatter::hostChanged(const QString &host, bool success) const
{
    QString s = "<i>" + QDateTime::currentDateTime().toString("yyyy.MM.dd hh.mm.ss") + "</i>\n";
    if(success) {
        s += "successfully set host to <b>" + host + "</b>";
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
