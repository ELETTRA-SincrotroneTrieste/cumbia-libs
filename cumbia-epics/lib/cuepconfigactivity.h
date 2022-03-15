#ifndef CUATTCONFIGACTIVITY_H
#define CUATTCONFIGACTIVITY_H

#include <cuactivity.h>

class CuEpConfigActivityPrivate;
class CuEpCAService;

class CuEpConfigActivity : public CuActivity
{
public:
    enum Type { CuEpConfigActivityType = CuActivity::UserAType + 5 };

    CuEpConfigActivity(const CuData& tok, CuEpCAService *df);

    void setDesiredAttributeProperties(const std::vector<std::string> &props);

    virtual ~CuEpConfigActivity();

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
    CuEpConfigActivityPrivate *d;
};

#endif // CUATTCONFIGACTIVITY_H
