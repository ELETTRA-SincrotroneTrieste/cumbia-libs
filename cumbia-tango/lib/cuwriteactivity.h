#ifndef CUWRITEACTIVITY_H
#define CUWRITEACTIVITY_H

#include <cuactivity.h>

class CuDeviceFactoryService;
class CuWriteActivityPrivate;

class CuWriteActivity : public CuActivity
{
public:
    enum Type { CuWriteA_Type = CuActivity::User + 32 };
    CuWriteActivity(const CuData &token,
                    CuDeviceFactoryService *df,
                    const CuData &db_config,
                    const CuData& tag);

    virtual ~CuWriteActivity();

    // CuActivity interface
public:
    void event(CuActivityEvent *e);
    bool matches(const CuData &token) const;

protected:
    void init();
    void execute();
    void onExit();

private:
    CuWriteActivityPrivate *d;

    // CuActivity interface
public:
    int getType() const;
    int repeat() const;
};

#endif // CUWRITEACTIVITY_H
