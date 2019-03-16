#include "botstats.h"
#include "botdb.h"
#include "botmonitor.h"
#include <cudata.h>
#include <QTimer>
#include <QDateTime>
#include <QMap>

class  BotStatsPrivate {
public:

    // readers map, count reads grouped by chat_id
    QMap<int, int> r_map;
    // errors per source
    QMap<QString, int> err_map;

    QDateTime start_dt, peak_dt;

    unsigned r_cnt, r_cnt_per_sec, max_r_per_sec;

    unsigned int alarm_rate;
};

BotStats::BotStats(QObject *parent) : QObject(parent)
{
    d = new BotStatsPrivate;
    reset();

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    timer->start(1000);
}

void BotStats::reset()
{
    d->r_cnt = d->r_cnt_per_sec = 0;
    d->max_r_per_sec = 0;
    d->alarm_rate = 100;
    d->start_dt = QDateTime::currentDateTime();
    d->peak_dt = d->start_dt;
}

void BotStats::addRead(int chat_id, const CuData &dat)
{
    d->r_cnt++;
    d->r_cnt_per_sec++;

    if(!d->r_map.contains(chat_id))
        d->r_map.insert(chat_id, 1);
    else
        d->r_map[chat_id]++;
    if(dat["err"].toBool()) {
        QString src = QString::fromStdString(dat["src"].toString());
        if(!d->err_map.contains(src))
            d->err_map.insert(src, 1);
        else {
            d->err_map[src]++;
        }
    }
}

void BotStats::setAlarmRate(unsigned int count)
{
    d->alarm_rate = count;
}

unsigned int BotStats::alarmRate() const
{
    return d->alarm_rate;
}

unsigned BotStats::read_cnt() const
{
    return  d->r_cnt;
}

const QMap<int, int> &BotStats::perUserReads() const
{
    return d->r_map;
}

QDateTime BotStats::startDt() const
{
    return d->start_dt;
}

QDateTime BotStats::peakDt() const
{
    return d->peak_dt;
}

void BotStats::onTimeout()
{
    if(d->max_r_per_sec < d->r_cnt_per_sec)
        d->max_r_per_sec = d->r_cnt_per_sec;

    if(d->r_cnt_per_sec > d->alarm_rate) {
        emit rateReached(d->r_cnt_per_sec);
        d->peak_dt = QDateTime::currentDateTime();
    }

    d->r_cnt_per_sec = 0;
}

QByteArray BotStatsFormatter::toJson(BotStats *stats, BotDb *db, const BotMonitor *botmon)
{
    QJsonObject jo;
    jo["read_cnt"] = static_cast<int>( stats->d->r_cnt);
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

    // active monitors
    if(botmon != nullptr)  { // null if no monitor started yet
        const QList<BotReader *> readers = botmon->readers();
        QJsonArray mon_srcs, mon_users, mon_ttl, mon_started, mon_types;
        QJsonArray mon_mode, mon_refcnt, mon_notifcnt, mon_formulas;
        foreach(const BotReader *r, readers) {
            mon_srcs.append(r->source());
            mon_users.append(usersMap.value(r->userId()));
            mon_ttl.append(r->ttl());
            mon_started.append(r->startedOn().toString("yyyy.MM.dd hh.mm.ss"));
            r->priority() == BotReader::Low ? mon_types.append("monitor") : mon_types.append("alert");
            // next is true if event refreshed
            mon_mode.append(r->refreshMode() == BotReader::Event);
            mon_refcnt.append(r->refreshCount());
            mon_notifcnt.append(r->notifyCount());
            mon_formulas.append(r->command());
        }
        if(readers.size() > 0) {
            jo["mon_srcs"] = mon_srcs;
            jo["mon_users"] = mon_users;
            jo["mon_ttl"] = mon_ttl;
            jo["mon_started"] = mon_started;
            jo["mon_types"] = mon_types;
            jo["mon_mode"] = mon_mode;
            jo["mon_refcnt"] = mon_refcnt;
            jo["mon_notifcnt"] = mon_notifcnt;
            jo["mon_formulas"] = mon_formulas;
        }
    }

    std::list<int> read_counters = stats->d->r_map.values().toStdList();
    qDebug() << __PRETTY_FUNCTION__ << usersMap << stats->d->r_map;
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
    return QJsonDocument(jo).toJson();
}
