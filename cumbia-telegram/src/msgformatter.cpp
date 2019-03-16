#include "msgformatter.h"
#include "formulahelper.h"
#include "botconfig.h"
#include <QDateTime>
#include <cudata.h>
#include <cudataquality.h>
#include <QFile>        // for help*.html in res/ resources
#include <QTextStream>  // for help*.html
#include <QtDebug>
#include <algorithm>

#define MAXVALUELEN 45

MsgFormatter::MsgFormatter()
{

}

QString MsgFormatter::lastOperation(const QDateTime &dt, const QString &name) const
{
    if(name.isEmpty())
        return "You haven't performed any operation yet!";
    QString msg = "<i>" + m_timeRepr(dt) + "</i>\n";
    msg += "operation: <b>" + FormulaHelper().escape(name) + "</b>";
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
            // 1. type + source [+formula if not empty]
            msg += QString::number(i+1) + ". "; // numbered list
            QString cmd = e.command;
            cmd = FormulaHelper().escape(cmd);
            msg += "<i>" + cmd + "</i>";

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
                    msg += QString(" stop [/X%1]").arg(e.index);

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
            msg+="\n\n";
        }
    }
//    printf("\e[1;34m%s\e[0m\n", qstoc(msg));
    return msg;
}

QString MsgFormatter::fromData(const CuData &d, MsgFormatter::FormatOption f)
{
    QString msg, eval_value;
    QString vector_info;

//    printf("\e[1;35mDATA %s\e[0m\n", d.toString().c_str());

    long int li = d["timestamp_ms"].toLongInt();
    QDateTime datet = QDateTime::fromMSecsSinceEpoch(li);
    bool ok = !d["err"].toBool();
    ok ? msg = "" : msg = "👎";

    QStringList pointSet, deviceSet, hostSet;

    if(d.containsKey("srcs")) {
        QString point, device, host;
        m_src = QString::fromStdString(d["srcs"].toString());
        QStringList srcs = m_src.split(",", QString::SkipEmptyParts);
        for(int i = 0; i < srcs.size(); i++) {
            m_cleanSource(srcs[i], point, device, host, f);
            if(!pointSet.contains(point)) pointSet << point;
            if(!deviceSet.contains(device)) deviceSet << device;
            if(!hostSet.contains(host)) hostSet << host;
        }
    }
    else {
        QString point, device, host;
        m_src = QString::fromStdString(d["src"].toString());
        m_cleanSource(m_src, point, device, host, f);
    }
    QString cmd = QString::fromStdString(d["command"].toString());

    if(!cmd.isEmpty())
        msg += "<i>" + FormulaHelper().escape(cmd) + "</i>:\n";

    // start with value, so that it is available in notification
    // preview

    // source: always


    if(!ok) {
        msg += "\n";
        msg += "😔  <i>" + QString::fromStdString(d["msg"].toString()) + "</i>\n";
    }
    else { // ok


        if(d.containsKey("value")) {
            bool ok;
            const CuVariant &va = d["value"];
            std::string print_format, value_str;

            d.containsKey("print_format") && !d.containsKey("values") ? value_str = va.toString(&ok, d["print_format"].toString().c_str()) :
                value_str = va.toString();
            QString v_str = QString::fromStdString(value_str);
            if(v_str.length() > MAXVALUELEN - 3) {
                v_str.truncate(MAXVALUELEN-3);
                v_str += "...";
            }
            eval_value = m_value = v_str;

            if(va.getSize() > 1) {
                vector_info = m_getVectorInfo(va);
            }
        }

        if(d.containsKey("evaluation"))
            eval_value = QString::fromStdString(d["evaluation"].toString());

        // value
        msg += "<b>" + eval_value + "</b>";

        // measurement unit if available
        QString du = QString::fromStdString(d["display_unit"].toString());
        if(!du.isEmpty())
            msg += " [" + QString::fromStdString(d["display_unit"].toString()) +  "]";

        eval_value.length() < 10 ? msg += "   " : msg += "\n";

        // if vector, provide some info about len, min and max
        // and then a link to plot it!
        if(!vector_info.isEmpty()) {
            msg += "\n" + vector_info + " /plot\n";
        }

        CuDataQuality quality(d["quality"].toInt());
        CuDataQuality::Type qt = quality.type();
        if(qt != CuDataQuality::Valid) {
            msg += "   ";
            m_quality = QString::fromStdString(quality.name());
            if(qt == CuDataQuality::Warning)
                msg += "😮   ";
            else if(qt == CuDataQuality::Alarm)
                msg += "😱   ";
            else if(qt == CuDataQuality::Invalid)
                msg += "👎   ";
            msg += "<i>" + m_quality + "</i>\n";
        }

        if(f > Short) {
            msg += "\ndata format: <i>" + QString::fromStdString(d["data_format_str"].toString()) + "</i>\n";
            msg += "\nmode:        <i>" + QString::fromStdString(d["mode"].toString()) + "</i>\n";
        }
    }

//    if(!msg.endsWith("\n"))
//        msg += "\n"; // new line

    // date time
    msg +=  "<i>" + m_timeRepr(datet) + "</i>";

    QString host = hostSet.join(", ");
    !host.isEmpty() ? msg+= " [<i>" + host + "</i>]" : msg += "";

    int idx = d["index"].toInt();

    //  /Xn command used to stop monitor
    if(idx > -1)
        msg += QString("   /X%1").arg(idx);


    return msg;
}

QString MsgFormatter::error(const QString &origin, const QString &message)
{
    QString msg;
    FormulaHelper fh;
    msg += "👎   " + fh.escape(origin) + ": <i>" + fh.escape(message) + "</i>";
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
        s = "formula <i>" + fh.escape(old) + "</i> has been <b>removed</b>";
    else if(old.isEmpty() && new_f.size() > 0)
        s = "formula <b>" + fh.escape(new_f)  + "</b> has been introduced";
    else
        s = "formula <i>" + fh.escape(old) + "</i>\nchanged into\n<b>" + fh.escape(new_f) + "</b>";

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
                        "<b>%2</b>").arg(FormulaHelper().escape(src)).arg(message);
    return s;
}

QString MsgFormatter::monitorUntil(const QString &src, const QDateTime &until) const
{
    QString m = FormulaHelper().escape(src);
    return QString("🕐   started monitoring <i>%1</i> until <i>%2</i>").arg(m).arg(m_timeRepr(until));
}

QString MsgFormatter::monitorStopped(const QString &cmd, const QString &msg) const
{
    FormulaHelper fh;
    QString m = fh.escape(cmd);
    return "stopped monitoring <i>" + m + "</i>: <i>" + fh.escape(msg) + "</i>";
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
        s += QString("<i>%1</i>\ntype: <i>%2</i>  [host: <i>%3</i>]").arg(b.command).arg(b.type).arg(b.host);
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

QString MsgFormatter::fromControlData(const ControlMsg::Type t, const QString &msg) const
{
    QString s;
    if(t == ControlMsg::Authorized) {
        s = "🎉   <b>congratulations</b>: you have been <b>authorized</b> to access the cumbia-telegram bot";
    }
    else if(t == ControlMsg::AuthRevoked) {
        s = "❌   your authorization to interact with the cumbia-telegram bot has been <b>revoked</b>";
    }
    return s;
}

QString MsgFormatter::aliasInsert(bool success, const QStringList& alias_parts, const QString &additional_message) const {
    QString s;
    success = success & alias_parts.size() > 2;
    QString desc, name, replaces;
    FormulaHelper fh;
    alias_parts.size() > 2 ? desc = fh.escape(alias_parts[2]) : desc = "";
    if(alias_parts.size() > 0)
        name = fh.escape(alias_parts[0]);
    if(success) {
        replaces = fh.escape(alias_parts[1]);
        s += "👍   successfully added alias:\n";
        s += QString("<b>%1</b> replaces: <i>%2</i>").arg(name).arg(replaces);
        if(!desc.isEmpty()) {
            s += "\n<i>" + desc + "</i>";
        }
    }
    else if(alias_parts.size() > 0) {
        s = "👎   failed to insert alias <b>" + name + "</b>";
    }
    if(!additional_message.isEmpty())
        s += "\n[<i>" + fh.escape(additional_message) + "</i>]";

    return s;
}

QString MsgFormatter::aliasList(const QString& name, const QList<AliasEntry> &alist) const
{
    FormulaHelper fh;
    QString s;
    if(alist.isEmpty())  {
        name.isEmpty()? s += "no alias defined" : s += "no alias defined for <i>" + name + "</i>.";
    }
    else {
        if(name.isEmpty()) s = "<b>alias list</b>\n";
    }
    int i = 1;
    foreach(AliasEntry a, alist) {
        s += QString("%1. <i>%2 --> %3</i>").arg(i).arg(fh.escape(a.name)).arg(fh.escape(a.replaces));
        for(int i = 0; i < a.in_history_idxs.size(); i++) {
            s += QString(" [/A%1] [%2]").arg(a.in_history_idxs[i]).arg(a.in_history_hosts[i]);
        }
        a.description.isEmpty() ? s += "\n" : s += "   (" + fh.escape(a.description) + ")\n";
    }
    return s;
}

QString MsgFormatter::botShutdown()
{
    return "😴   bot has gone to <i>sleep</i>\n"
           "Monitors and alerts will be suspended";
}

QString MsgFormatter::help(TBotMsgDecoder::Type t) const {
    QString h;
    QString f = ":/help/res/";
    switch(t) {
    case TBotMsgDecoder::Help:
        f += "help.html";
        break;
    case TBotMsgDecoder::HelpMonitor:
        f += "help_monitor.html";
        break;
    case TBotMsgDecoder::HelpAlerts:
        f += "help_alerts.html";
        break;
    case TBotMsgDecoder::HelpSearch:
        f += "help_search.html";
        break;
    case TBotMsgDecoder::HelpHost:
        f += "help_host.html";
        break;
    default:
        break;
    }
    if(!f.isEmpty()) {
        QFile hf(f);
        if(hf.open(QIODevice::ReadOnly)) {
            QTextStream in(&hf);
            h = "📚   " + in.readAll();
            hf.close();
        }
        else {
            perr("MsgFormatter.help: failed to open file \"%s\" in read only mode: %s",
                 qstoc(f), qstoc(hf.errorString()));
            h += "😞   error getting help: internal error";
        }
    }

    return h;
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

QString MsgFormatter::m_getVectorInfo(const CuVariant &v)
{
    QString s;
    std::vector<double> vd;
    v.toVector<double>(vd);
    auto minmax = std::minmax_element(vd.begin(),vd.end());
    s += QString("vector size: <b>%1</b> min: <b>%2</b> max: <b>%3</b>").arg(vd.size())
            .arg(*minmax.first).arg(*minmax.second);
    return s;
}

void MsgFormatter::m_cleanSource(const QString &src, QString& point, QString& device,QString&  host, MsgFormatter::FormatOption f) const
{
    QString  s(src);
    if(s.count('/') > 3)
        host = s.section('/', 0, 0);
    if(f <= Short && s.count('/') > 3) // remove host:PORT/ so that src is not too long
        s.replace(0, s.indexOf('/', 0) + 1, "");

    if(s.count('/') > 1) {
        point = s.section('/', -1);
        device = s.section('/', 0, 2);
    }
}
