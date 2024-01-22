#include "myactivitylistener.h"
#include <cudata.h>

MyActivityListener::MyActivityListener(Stats *stats) : update_cnt{0}, m_stats(stats) {

}

void MyActivityListener::onProgress(int step, int total, const CuData &data)
{
}

void MyActivityListener::onResult(const CuData &data) {
    m_stats->add(data.u("idx"), data.u("step"), data.i("sleep_ms"));
}

void MyActivityListener::onResult(const std::vector<CuData> &datalist)
{
}

CuData MyActivityListener::getToken() const
{
}
