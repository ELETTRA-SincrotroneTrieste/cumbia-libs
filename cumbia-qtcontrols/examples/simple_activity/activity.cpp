#include "activity.h"

Activity::Activity(const CuData& token)
    : CuIsolatedActivity(token) {

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
    d["thread"] = pthread_self(); // type pthread_t (unsigned long int)
    publishResult(d); // post to main thread
}

void Activity::execute()
{
    CuData d = getToken();
    for(int i = 1; i <= 3; i++) {
        d["msg"] = std::string("execute [") + std::to_string(i) + std::string("]");
        d["thread"] = pthread_self(); // unsigned long int
        publishProgress(i, 3, d);
    }
    d["thread"] = pthread_self();
    d["msg"] = std::string("execute [done]");
    publishResult(d);
}

void Activity::onExit()
{
    // do exactly the same as init
    CuData d = getToken();
    d["msg"] = std::string("onExit");
    d["thread"] = pthread_self();
    publishResult(d);
}
