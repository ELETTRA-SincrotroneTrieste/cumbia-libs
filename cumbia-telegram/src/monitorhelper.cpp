#include "monitorhelper.h"
#include "botmonitor.h"
#include <QMultiMap>
#include <QSet>

MonitorHelper::MonitorHelper()
{

}

void MonitorHelper::adjustPollers(BotMonitor *mon, int desired_poll_period, int max_avg_period)
{
    QSet<int> uids;
    QMultiMap<int, BotReader *> readersMap;
    QList<BotReader *>  readers = mon->readers();
    printf("\e[0;33;4mmMonitorHelper.adjustPollers\e[0m there are %d readers\n", readers.size());
    foreach(BotReader *r, readers) {
        if(r->refreshMode() == BotReader::Polled) {
            printf("\e[0;33mMonitorHelper.adjustPollers: source %s is polled\e[0m\n", qstoc(r->source()));
            readersMap.insertMulti(r->userId(), r);
            uids.insert(r->userId());
        }
        else {
            printf("\e[0;33mMonitorHelper.adjustPollers: source \"%s\" is not polled [%d]\n", qstoc(r->source()),
                   (r->refreshMode()));
        }
    }

    int users_count = uids.size();
    int per_user_poll_max = max_avg_period * users_count;
    int period, per_user_per_reader_period;

    foreach(int uid, uids) {
        int per_u_sources = 0;
        QList<BotReader *> per_u_readers = readersMap.values(uid);
        for(int i = 0; i < per_u_readers.size(); i++)
            per_u_sources += per_u_readers[i]->sources().size();
        per_user_per_reader_period = per_user_poll_max * per_u_sources;
        desired_poll_period < per_user_per_reader_period ?  period = per_user_per_reader_period
                : period = desired_poll_period;
        foreach(BotReader *r, per_u_readers) {
            printf("\e[0;33mMonitorHelper.adjustPollers: source %s desired %d per user %dms per user and per reader %dms-- \e[1;32meffective %d\e[0m\n",
                   qstoc(r->source()), desired_poll_period, per_user_poll_max, per_user_per_reader_period, period);
            r->setPeriod(period);
        }
    }

}
