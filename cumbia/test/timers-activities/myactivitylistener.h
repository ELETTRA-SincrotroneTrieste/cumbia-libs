#ifndef MYTIMERLISTNER_H
#define MYTIMERLISTNER_H

#include <cuhltimer.h>
#include <cuthreadlistener.h>

#include "stats.h"

class MyActivityListener : public CuThreadListener
{
public:
    MyActivityListener(Stats * stats);

private:
    size_t update_cnt;
    Stats* m_stats;

    // CuThreadListener interface
public:
    void onProgress(int step, int total, const CuData &data);
    void onResult(const CuData &data);
    void onResult(const std::vector<CuData> &datalist);
    CuData getToken() const;
};

#endif // MYTIMERLISTNER_H
