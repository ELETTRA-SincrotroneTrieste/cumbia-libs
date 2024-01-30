#ifndef STATS_H
#define STATS_H
#include <vector>
#include <string>
#include <chrono>
#include <cuhltimer.h>

class StatsData {
public:
    int step;
    int activity_sleep_ms;
};

class Stats : public CuHLTimerListener
{
public:
    Stats(int activity_cnt, int period);
    void add(int idx, unsigned step, int activity_sleep_millis);

    void dump();

    std::vector<StatsData> statsd;
    std::vector<unsigned> exec_cnt, prev_xec_cnt;
    std::chrono::milliseconds last_dump;
    int m_period;

    // CuHLTimerListener interface
public:
    void onTimeout(unsigned int period);
};

#endif // STATS_H
