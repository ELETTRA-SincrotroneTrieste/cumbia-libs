#ifndef CUEVENTACTIVITY_H
#define CUEVENTACTIVITY_H

#include <cuactivity.h>
#include <cuactivityevent.h>
#include <tango.h>

class CuEventActivityPrivate;
class CuDeviceFactoryService;

class CuTAStopEvent : public CuActivityEvent
{
public:


    // CuActivityEvent interface
public:
    Type getType() const;
};

class CuEventActivity : public CuActivity, public Tango::CallBack
{
public:
    enum Type { CuEventActivityType = CuActivity::User + 2 };

    CuEventActivity(const CuData& token, CuDeviceFactoryService *df);

    ~CuEventActivity();

    // CuActivity interface
public:
    int getType() const;
    void event(CuActivityEvent *e);
    bool matches(const CuData &token) const;
    int repeat() const;

protected:
    void init();
    void execute();
    void onExit();

    void push_event(Tango::EventData *);

private:
    CuEventActivityPrivate *d;

    Tango::EventType m_tevent_type_from_string(const std::string& set) const;
};

#endif // CUEVENTACTIVITY_H
