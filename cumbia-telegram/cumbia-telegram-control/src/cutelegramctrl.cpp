#include "cutelegramctrl.h"
#include "../cumbia-telegram-defs.h"
#include "dbctrl.h"
#include <QLocalSocket>
#include <QMap>
#include <QList>
#include <QtDebug>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDateTime>

class CuTelegramCtrlPrivate {
public:
    QLocalSocket *so;
    bool error;
    QString msg;
    DbCtrl *db;
};

CuTelegramCtrl::CuTelegramCtrl(QObject *parent) : QObject(parent)
{
    d = new CuTelegramCtrlPrivate;
    d->so = new QLocalSocket(this);
    connect(d->so, SIGNAL(error(QLocalSocket::LocalSocketError)), this,
            SLOT(onLocalSocketError(QLocalSocket::LocalSocketError)));
    d->error = false;
    d->db = nullptr;
}

bool CuTelegramCtrl::init(const QString &servernam, DbCtrl *db)
{
    d->db = db;
    d->so->connectToServer(servernam);
    d->error = !d->so->waitForConnected();
    if(d->error) {
        d->msg = QString("error opening local socket in RW mode (name %1): %2")
                .arg(servernam).arg(d->so->errorString());
        print_error("CuTelegramCtrl.init", d->msg);
    }
    return !d->error;
}

CuTelegramCtrl::~CuTelegramCtrl()
{
    if(d->so && d->so->isOpen())
        d->so->close();
    delete d;
}

bool CuTelegramCtrl::error() const
{
    return d->error;
}

bool CuTelegramCtrl::printUsers()
{
    QList<QMap <QString, QString> > umap;
    if(d->db) {
        QString s1, s2;
        QStringList keys = QStringList() << "id" << "uname" << "first_name" << "last_name" << "join_date";
        d->db->getUserInfo(umap);
        for(int i = 0; i < umap.size(); i++) {
            const QMap<QString, QString> &ma = umap[i];
            int auth = ma["authorized"].toInt();
            if(auth == 0) {
                s1 = "\e[1;31mX\e[0m";
                s2 = "\e[1;31;47;3mUNAUTHORIZED\e[0m";
            }
            else if(auth > 0) {
                s1 = "\e[1;32mOK\e[0m";
                s2 = "\e[1;32;47mAUTHORIZED\e[0m";
            }
            else {
                s1 = "\e[1;35m?\e[0m";
                s2 = "\e[0;35;47;3mAUTH PENDING\e[0m";
            }
            printf("%s\t", s1.toUtf8().data());
            foreach(const QString& s, keys) {
                printf("%s\t",  ma[s].toUtf8().data());
            }
            printf("%s\t\n", s2.toUtf8().data());
        }
    }
    return !d->db->error();
}

QString CuTelegramCtrl::message() const
{
    return d->msg;
}
void CuTelegramCtrl::authorize(int uid, bool authorize)
{
    if(!d->db)
        return;
    print(QString("setting authorization to %1 for user id %2").arg(authorize).arg(uid));
    bool success = d->db->setAuthorized(uid, authorize);
    print_success(success);
    if(!success)
        print_error("DbCtrl", d->db->message());
    else if(d->db->hasMessage()) {
        printf("\e[1;32m-->\e[0m %s\n", d->db->message().toUtf8().data());
        print(QString("notifying the change to user id %1 ").arg(uid));
        QString mess;
        QJsonObject json;
        json["uid"] = uid;
        json["reply"] = false;
        authorize ? json["ctrl_type"] = ControlMsg::Authorized
                : json["ctrl_type"] = ControlMsg::AuthRevoked;

        success = send(QString(QJsonDocument(json).toJson()), false);
        print_success(success);
    }
}

/*
 * QJsonObject jo;
    jo["reads"] = static_cast<int>( stats->d->r_cnt);
    jo["since"] = stats->startDt().toString("yyyy.MM.dd hh.mm.ss");
    jo["peak"] = static_cast<int>(stats->d->max_r_per_sec);
    jo["peak_dt"] = stats->d->peak_dt.toString("yyyy.MM.dd hh.mm.ss");

    if(stats->d->err_map.size() > 0) {
        QJsonArray err_srcs, err_cnts;
        foreach(QString s, stats->d->err_map.keys()) {
            err_cnts.append(stats->d->err_map[s]);
            err_srcs.append(s);
        }
        jo["err_srcs"] = err_srcs;
        jo["err_cnts"] = err_cnts;
    }

    QMap<int, QString> usersMap = db->usersById();
    std::list<int> read_counters = stats->d->r_map.values().toStdList();
    if(read_counters.size() > 0 && usersMap.size() > 0) {
        QJsonArray users, reads;
        read_counters.sort();
        for(std::list<int>::const_iterator it = read_counters.begin(); it != read_counters.end(); ++it) {
            users.append(usersMap[stats->d->r_map.key(*it)]);
            reads.append(*it);
        }
        jo["users"] = users;
        jo["reads"] = reads;
    }
 */
void CuTelegramCtrl::printStats(const QString &s) const
{
    QJsonDocument jd = QJsonDocument::fromJson(s.toUtf8());
    int readcnt = jd["read_cnt"].toInt();
    QString since = jd["since"].toString(); // yyyy.MM.dd hh.mm.ss
    int peak = jd["peak"].toInt(); // peak per second
    QString peak_dt = jd["peak_dt"].toString(); // yyyy.MM.dd hh.mm.ss date time of peak

    // 1. generic statistics

    QString uptime;
    QDateTime now = QDateTime::currentDateTime();
    QDateTime startDt = QDateTime::fromString(since, "yyyy.MM.dd hh.mm.ss");
    qint64 secs = startDt.secsTo(now);
    qint64 days = startDt.daysTo(now);
    const qint64 secsPerDay = 3600 * 24;
    secs = secs - days * secsPerDay;
    qint64 hours = secs / 3600;
    secs -= hours * 3600;
    qint64 mins = secs / 60;
    secs -= mins * 60;

    if(days > 0) uptime = QString("%1 d ").arg(days);
    if(hours > 0) uptime += QString("%1h ").arg(hours);
    if(mins > 0) uptime += QString("%1min ").arg(mins);
    uptime += QString("%1s ").arg(secs);

    printf("|\e[1;32m%-20s\e[0m|\e[1;32m%-20s\e[0m|\n", "UPTIME", uptime.toUtf8().data());

    printf("\n\e[1;36;4mREADS\e[0m\n\n");
    printf("|%-20s|%-20s|\e[1;32m%12d\e[0m|\n", "Total reads since", since.toStdString().c_str(), readcnt);
    printf("|%-20s|%-20s|\e[1;31m%12d\e[0m|\n", "Peak (reads/sec)", peak_dt.toStdString().c_str(), peak);

    // 2. per user readings

    QJsonArray users = jd["users"].toArray(); // list of user names
    QJsonArray reads = jd["reads"].toArray(); // read count per user
    if(users.size() > 0 && users.size() == reads.size()) {

        printf("\n\e[1;36;4mUSERS\e[0m\n\n");
        // header
        printf("|%-20s|%-20s|%12s\e[0m|\n", "User No.", "User name", "Read count");
        printf("|%-20s|%-20s|%12s\e[0m|\n", "", "", "");

        for(int i = 0; i < users.size(); i++) {
            printf("|%-20d|%-20s|\e[1;32m%12d\e[0m|\n", i+1,
                   users.at(i).toString().toStdString().c_str(), reads.at(i).toInt());
        }
    }

    // 3. monitors
    // jo["mon_srcs"] = mon_srcs;
    // jo["mon_users"] = mon_users;
    // jo["mon_ttl"] = mon_ttl;
    //  jo["mon_started"] = mon_started;
    QJsonArray mon_srcs, mon_users, mon_ttl, mon_started, mon_types;
    QJsonArray mon_mode, mon_refcnt, mon_notifcnt, mon_formulas;
    mon_srcs = jd["mon_srcs"].toArray();
    mon_users = jd["mon_users"].toArray();
    mon_ttl = jd["mon_ttl"].toArray();
    mon_started   = jd["mon_started"].toArray();
    mon_types = jd["mon_types"].toArray();
    mon_mode = jd["mon_mode"].toArray();
    mon_refcnt =jd["mon_refcnt"].toArray();
    mon_notifcnt = jd["mon_notifcnt"].toArray();
    mon_formulas = jd["mon_formulas"].toArray();

    int siz = mon_srcs.size(); // size check
    if(siz > 0 && siz == mon_users.size() && siz == mon_ttl.size() &&
            mon_started.size() == siz && mon_mode.size() == siz &&
            mon_refcnt.size() == siz && mon_notifcnt.size() == siz &&
            siz == mon_formulas.size() && siz == mon_types.size()) {
        // header
        printf("\n\e[1;36;4mMONITORS\e[0m\n\n");

        printf("|%-42s|%-16s|%-7s|%-20s|%-20s|%6s|%8s|%8s|%8s|%15s|\n", "source", "username", "type",
               "start date/time",
               "stop  date/time", "TTL[s]", "mode", "ref.cnt", "notifcnt", "formula");
        printf("|%-42s|%-16s|%-7s|%-20s|%-20s|%6s|%8s|%8s|%8s|%15s|\n", "", "", "", "", "", "", "", "", "", "");
        for(int i = 0; i < mon_srcs.size(); i++) {
            QString trunc_formula(mon_formulas.at(i).toString());
            if(trunc_formula.length() > 15) {
                trunc_formula.truncate(12);
                trunc_formula += "...";
            }
            if(trunc_formula.isEmpty())
                trunc_formula = "-";

            QDateTime started = QDateTime::fromString(mon_started.at(i).toString(), "yyyy.MM.dd hh.mm.ss");
            QDateTime stopdt = started.addSecs(mon_ttl.at(i).toInt());
            printf("|%-42s|%-16s|%-7s|%-20s|%-20s|%6d|%8s|%8d|%8d|%15s|\n", mon_srcs.at(i).toString().toStdString().c_str(),
                   mon_users.at(i).toString().toStdString().c_str(),
                   mon_types.at(i).toString().toUtf8().data(),
                   mon_started.at(i).toString().toStdString().c_str(),
                   stopdt.toString("yyyy.MM.dd hh.mm.ss").toStdString().c_str(),
                   mon_ttl.at(i).toInt(), mon_mode.at(i).toString("-").toStdString().c_str(),
                   mon_refcnt.at(i).toInt(), mon_notifcnt.at(i).toInt(), trunc_formula.toUtf8().data());
        }
    }
    // 4. errors
    QJsonArray err_srcs = jd["err_srcs"].toArray(); // list of sources with errors
    QJsonArray err_cnts = jd["err_cnts"].toArray(); // error count for each source above
    if(err_cnts.size() > 0 && err_cnts.size() == err_srcs.size()) {
        printf("\n\e[1;31;4mERRORS\e[0m\n\n");
        // use longer padding for source
        // header
        printf("|%-20s|%-50s|%12s\e[0m|\n", "User No.", "Source", "Error count"); // error count len 11
        printf("|%-20s|%-50s|%12s\e[0m|\n", "", "", "");
        // error list
        for(int i = 0; i < err_srcs.size(); i++) {
            printf("|%-20d|%-50s|\e[1;31m%12d\e[0m|\n", i+1,
                   err_srcs.at(i).toString().toStdString().c_str(),
                   err_cnts.at(i).toInt());
        }
    }
}

bool CuTelegramCtrl::requestStats()
{
    bool has_reply = true;
    bool success;
    QString mess;
    QJsonObject json;
    json["reply"] = true;
    json["ctrl_type"] = ControlMsg::Statistics;
    success = send(QString(QJsonDocument(json).toJson()), has_reply);
    return success;
}

bool CuTelegramCtrl::send(const QString &s, bool wait_for_reply)
{
    qint64 bytes_written = d->so->write(s.toUtf8());
    d->error = bytes_written < 0;
    if(!d->error) {
        d->error = !d->so->waitForBytesWritten();
        if(!d->error && wait_for_reply) {
            d->error = !d->so->waitForReadyRead();
            if(!d->error) {
                onNewData(d->so->readAll());
            }
        }
    }
    if(d->error) {
        d->msg = d->so->errorString();
        print_error("CuTelegramCtrl.send", d->msg);
    }
    return !d->error;
}

void CuTelegramCtrl::onNewData(const QByteArray &ba)
{
    QJsonDocument doc = QJsonDocument::fromJson(ba);
    QString msg = doc["msg"].toString(); // another json doc

    ControlMsg::Type t = static_cast<ControlMsg::Type>(doc["ctrl_type"].toInt());
    // print sender info
    QString sh = m_senderHeader(doc.object());
    printf("\n\e[1;4mSTATISTICS\e[0m from \e[0;34m%s\e[0m:\n\n", sh.toStdString().c_str());
    if(t == ControlMsg::Statistics)
        printStats(msg);
}

void CuTelegramCtrl::onLocalSocketError(QLocalSocket::LocalSocketError e)
{
    Q_UNUSED(e);
    d->error = true;
    d->msg = qobject_cast<QLocalSocket *>(sender())->errorString();
}

void CuTelegramCtrl::m_printReply(const QString &r)
{

}

QString CuTelegramCtrl::m_senderHeader(const QJsonObject &o)
{
    QString ret;
    QString sender = o["sender"].toString();
    int pid = o["sender_pid"].toInt(-1);
    QString cmdline = o["sender_cmd_line"].toString();
    ret = QString("%1 [pid:%2] [%3]").arg(sender).arg(pid).arg(cmdline);
    return ret;
}

void CuTelegramCtrl::print_error(const QString &origin, const QString &msg)
{
    fprintf(stderr, "\e[1;31;4merror\e[0m: %s: \e[1m%s\e[0m\n", origin.toUtf8().data(), msg.toUtf8().data());
}

void CuTelegramCtrl::print_error()
{
    print_error("CuTelegramCtrl", d->msg);
}

void CuTelegramCtrl::print(const QString &s)
{
    printf("%s...", s.toUtf8().data());
    fflush(stdout);
}

void CuTelegramCtrl::print_success(bool ok)
{
    ok? printf("\t\t[\e[1;32mOK\e[0m]\n") : printf("\t\t[\e[1;31mFAILED\e[0m]\n");
}

