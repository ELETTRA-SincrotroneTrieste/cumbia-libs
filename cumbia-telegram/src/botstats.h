#ifndef BOTSTATS_H
#define BOTSTATS_H

#include <QObject>
#include <QDateTime>

class CuData;
class BotDb;
class BotStatsPrivate;
class BotMonitor;

class BotStats : public QObject
{
    friend class BotStatsFormatter;

    Q_OBJECT
public:
    explicit BotStats(QObject *parent);

    void addRead(int chat_id, const CuData& dat);

    void setAlarmRate(unsigned int count);

    unsigned int alarmRate() const;

    unsigned read_cnt() const;

    const QMap<int, int>& perUserReads() const;

    QDateTime startDt() const;

    QDateTime peakDt() const;

signals:
    void rateReached(unsigned int reads_per_sec);

public slots:
    void reset();

private slots:
    void onTimeout();

private:
    BotStatsPrivate *d;
};

class BotStatsFormatter {

public:
    QByteArray toJson(BotStats *stats, BotDb *db, const BotMonitor *botmon);
};

#endif // STATS_H
