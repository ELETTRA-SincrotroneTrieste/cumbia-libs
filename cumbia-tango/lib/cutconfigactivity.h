#ifndef CUATTCONFIGACTIVITY_H
#define CUATTCONFIGACTIVITY_H

#include <cuisolatedactivity.h>

class CuTAttConfigActivityPrivate;
class CuDeviceFactoryService;

class CuTConfigActivity : public CuActivity
{
public:
    enum Type { CuReaderConfigActivityType = CuActivity::User + 5,  CuWriterConfigActivityType };

    CuTConfigActivity(const CuData& tok, CuDeviceFactoryService *df, Type t);

    void setDesiredAttributeProperties(const std::vector<std::string> &props);

    virtual ~CuTConfigActivity();

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

private:
    CuTAttConfigActivityPrivate *d;
};

#endif // CUATTCONFIGACTIVITY_H
