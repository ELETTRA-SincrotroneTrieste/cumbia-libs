#include "stats.h"

Stats::Stats(int activity_cnt, int period) {
    statsd.resize(activity_cnt);
    for(size_t i = 0; i < activity_cnt; i++) {
        prev_xec_cnt.push_back(0);
        exec_cnt.push_back(0);
    }
    last_dump = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch());
    m_period = period;
}

void Stats::add(int idx, unsigned int step, int activity_sleep_millis) {
    statsd[idx].step = step;
    statsd[idx].activity_sleep_ms = activity_sleep_millis;
    exec_cnt[idx]++;
}

void Stats::dump() {
    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch());
    time_t ms = (now - last_dump).count();
    float expected_xec = ms / m_period, percent;
    for(size_t i = 0; i < statsd.size(); i++) {
        const unsigned &xcnt = exec_cnt[i] - prev_xec_cnt[i];
        percent = 100.0 * xcnt / expected_xec;
        printf("\e[1;32m#\e[0m activity %ld (sleeping for %dms): value %d executed %u times in the last %ldms [expected %.1f times \e[1;32m%.1f%%]\e[0m\n",
               i+1, statsd[i].activity_sleep_ms, statsd[i].step, xcnt, ms, expected_xec, percent);
        prev_xec_cnt[i] = exec_cnt[i];
    }
    last_dump = now;
}

void Stats::onTimeout(unsigned int period) {
    dump();
}
