#ifndef CUATTCONFIGACTIVITY_H
#define CUATTCONFIGACTIVITY_H

#include <cuactivity.h>
#include <cutconfigactivity_executor_i.h>

class CuTAttConfigActivityPrivate;
class CuDeviceFactoryService;
class TSource;

/*!
 * \brief Activity to get attribute configuration or command info. Optionally, it can perform a one
 *        shot attribute read (or command inout) if the option *value-only* is set to true
 */
class CuTConfigActivity : public CuActivity
{
public:
    enum Type { CuReaderConfigActivityType = CuActivity::User + 5,  CuWriterConfigActivityType };

    CuTConfigActivity(const TSource &ts,
                      CuDeviceFactoryService *df,
                      Type t,
                      const CuTConfigActivityExecutor_I *tx,
                      const CuData &o,
                      const CuData &tag);
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

class CuTConfigActivityExecutor_Default : public CuTConfigActivityExecutor_I {
public:
    virtual ~CuTConfigActivityExecutor_Default() {}
    bool get_command_info(Tango::DeviceProxy *dev, const std::string& cmd, CuData &cmd_info) const;
    bool get_att_config(Tango::DeviceProxy *dev,
                        const std::string &attribute,
                        CuData &dres,
                        bool skip_read_att,
                        const std::string &devnam) const;
};


#endif // CUATTCONFIGACTIVITY_H
