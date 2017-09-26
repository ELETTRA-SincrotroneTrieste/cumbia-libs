#ifndef CUPOLLINGACTIVITY_H
#define CUPOLLINGACTIVITY_H

#include <cucontinuousactivity.h>
#include <list>

class CuData;
class CuPollingActivityPrivate;
class CuDeviceFactoryService;

class CuPollingActivity : public CuContinuousActivity
{
public:
    enum Type { CuPollingActivityType = CuActivity::User + 3 };

    CuPollingActivity(const CuData& token, CuDeviceFactoryService *df, const CuVariant &argins =
            std::vector<std::string>());

    ~CuPollingActivity();

    void setArgins(const CuVariant &argins);

    // CuActivity interface
public:
    bool matches(const CuData &token) const;

protected:
    void init();
    void execute();
    void onExit();

private:
    CuPollingActivityPrivate *d;

    // CuActivity interface
public:
    void event(CuActivityEvent *e);

    int getType() const;

    int repeat() const;
};

#endif // CUPOLLINGACTIVITY_H
