#include "msgformatter.h"
#include <QDateTime>
#include <cudata.h>

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

QString MsgFormatter::history(const QStringList &cmd_shortcuts, const QStringList &timestamps, const QStringList &cmds) const
{
    QString msg;
    if(cmd_shortcuts.size() == 0)
        msg = "history still empty!";
    else if(cmd_shortcuts.size() == timestamps.size() && timestamps.size() == cmds.size()) {
        for(int i = 0; i < cmds.size(); i++) {
            msg +=  cmd_shortcuts[i] + "\n"
                    "<i>" + timestamps[i] + "</i>\n"
                    "<b>" + cmds[i] + "</b>\n";
        }
    }
    return msg;
}

QString MsgFormatter::fromData(const CuData &d, MsgFormatter::FormatOption f)
{
    QString msg;
    QString src = QString::fromStdString(d["src"].toString());
    long int li = d["timestamp_ms"].toLongInt();
    QString ts = QDateTime::fromMSecsSinceEpoch(li).toString("yyyy.MM.dd hh:mm:ss");
    bool ok = !d["err"].toBool();

    // start with timestamp, always
    ok ? msg = "👍  " : msg = "👎";
    msg +=  "<i>" +ts + "</i>\n";
    // source: always
    msg += "<i>" + src + "</i>:";

    if(!ok) {
        msg += "\n";
        msg += "😔  <i>" + QString::fromStdString(d["msg"].toString()) + "</i>";
    }
    else { // ok


        QString v = QString::fromStdString(d["value"].toString());
        // value
        v.length() < 10 ? msg += " " : msg += "\n";
        msg += "<b>" + v + "</b>";

        // measurement unit if available
        if(v.contains("display_unit"))
            msg += " [" + QString::fromStdString(d["display_unit"].toString()) +  "]\n";

        int quality = d["quality"].toInt();
        if(quality != 0) {
            msg += "\n";
            CuTangoWorldConfig twc;
            QString qual_s = QString::fromStdString(twc.qualityString(static_cast<Tango::AttrQuality>(quality)));
            if(qual_s.compare("ATTR_WARNING", Qt::CaseInsensitive) == 0)
                msg += "😮   <i>warning</i>\n";
            else if(qual_s.compare("ATTR_ALARM", Qt::CaseInsensitive) == 0)
                msg += "😱   <i>alarm</i>\n";
            else if(qual_s.compare("ATTR_INVALID", Qt::CaseInsensitive) == 0)
                msg += "👎   <i>invalid</i>\n";
            else
                msg += "quality: <i>" + qual_s + "</i>\n";
        }

        if(f > Short) {
            msg += "\ndata format: <i>" + QString::fromStdString(d["data_format_str"].toString()) + "</i>\n";
            msg += "\nmode:        <i>" + QString::fromStdString(d["mode"].toString()) + "</i>\n";
        }
    }



    return msg;

}
