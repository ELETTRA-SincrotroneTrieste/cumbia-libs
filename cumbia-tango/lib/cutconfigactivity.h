#ifndef CUATTCONFIGACTIVITY_H
#define CUATTCONFIGACTIVITY_H

#include <cuisolatedactivity.h>

class CuTAttConfigActivityPrivate;
class CuDeviceFactoryService;

/*!
 * \brief Activity to get attribute configuration or command info. Optionally, it can perform a one
 *        shot attribute read (or command inout) if the option *value-only* is set to true
 */
class CuTConfigActivity : public CuActivity
{
public:
    enum Type { CuReaderConfigActivityType = CuActivity::User + 5,  CuWriterConfigActivityType };

    CuTConfigActivity(const CuData& tok, CuDeviceFactoryService *df, Type t);

    virtual ~CuTConfigActivity();

    void setOptions(const CuData& o);

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
