#include "activity.h"
#include <unistd.h>

Activity::Activity(const CuData& token)
    : CuIsolatedActivity(token) {
    printf("created activity: flags %d\n", getFlags());
}

int Activity::getType() const {
    return CuActivity::Isolated;
}

void Activity::event(CuActivityEvent *) {

}

bool Activity::matches(const CuData &token) const {
    return getToken() == token;
}

void Activity::init()
{
    // result: a CuData.
    // pick the token to initialise the result
    CuData d = getToken();
    d["msg"] = std::string("init"); // insert the message into the result
    d["thread"] = write_thread_t();
    publishResult(d); // post to main thread
}

void Activity::execute()
{
    CuData d = getToken();
    for(int i = 1; i <= 3; i++) {
        d["msg"] = std::string("execute [") + std::to_string(i) + std::string("]");
        d["thread"] = write_thread_t();
        publishProgress(i, 3, d);
        if (i < 3) sleep(2);
    }
    d["thread"] = write_thread_t();
    d["msg"] = std::string("execute [done]");
    publishResult(d);
}

void Activity::onExit()
{
    // do exactly the same as init
    CuData d = getToken();
    d["msg"] = std::string("onExit");
    d["thread"] = write_thread_t();
    publishResult(d);
    printf("onExit activity: flags %d\n", getFlags());
}

std::string Activity::write_thread_t() const
{
    char th[32];
    snprintf(th, 32, "0x%lx", pthread_self());
    return std::string(th);
}
