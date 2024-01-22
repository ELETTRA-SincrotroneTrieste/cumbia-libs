#include "myactivity.h"
#include <thread>

MyActivity::MyActivity(int idx, bool verbose, int sleep_ms) {
    m_idx = idx;
    m_verbose = verbose;
    m_sleep_ms = sleep_ms;
}

bool MyActivity::matches(const CuData &token) const {
    return token == this->getToken();
}

void MyActivity::init() {
    m_step = 0;
}

void MyActivity::execute() {
    if(m_verbose)
        printf("activity %u: step %u (this activity sleeps for %dms)\e[0m\n", m_idx, m_step, m_sleep_ms);
    std::this_thread::sleep_for(std::chrono::milliseconds(m_sleep_ms));
    publishResult(CuData("step", ++m_step).set("idx", m_idx).set("sleep_ms", m_sleep_ms));
}

void MyActivity::onExit() {
    pretty_pri("activity %d exited after executing %d times", m_idx, m_step);
}
