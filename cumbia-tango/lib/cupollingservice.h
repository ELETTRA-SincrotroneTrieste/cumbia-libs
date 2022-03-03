#ifndef CUPOLLINGSERVICE_H
#define CUPOLLINGSERVICE_H

#include <cuservicei.h>
#include "cupolldataupdatepolicy_enum.h"

class CuPoller;
class CuPollingServicePrivate;
class CumbiaTango;
class TSource;
class CuTangoActionI;
class CuData;

/*! \brief CumbiaTango polling service. Mainly used internally, it can be accessed to specify a particular data update policy.
 *
 * \par Update policy while data remains constant.
 *
 *  CuPollingService can now avoid publishing results when data does not change. This can save a lot of time to applications
 *  reading from several *polled sources*.
 *
 *  Three types of *update policy* are available for *polled sources*:
 *
 *   1. Always update, even though the data does not change (default usual behaviour)
 *   2. Update the timestamp only as long as data does not change;
 *   3. Do not perform any update as long as data does not change. Updates are not sent
 *      at the *activity* level. This means event posting on the application thread is
 *      completely skipped, ensuring maximum performance.
 *
 *  Changes in error flags, messages or Tango data quality are always notified.
 *
 *  The update policy shall be specified using setDataUpdatePolicy
 *
 *  @see CuPollDataUpdatePolicy
*/
class CuPollingService : public CuServiceI
{
public:
    enum Type { CuPollingServiceType = CuServices::User + 26 };

    CuPollingService();
    virtual ~CuPollingService();

    CuPoller *getPoller(CumbiaTango *cu_t, int period);

    void registerAction(CumbiaTango* ct, const TSource& tsrc, int period, CuTangoActionI *action, const CuData &options, const CuData &tag);
    void unregisterAction(int period, CuTangoActionI *action);
    bool actionRegistered(CuTangoActionI *ac, int period);

    void setDataUpdatePolicy(CuPollDataUpdatePolicy p);
    CuPollDataUpdatePolicy dataUpdatePolicy() const;

    // CuServiceI interface
    virtual std::string getName() const;
    virtual CuServices::Type getType() const;

private:
    CuPollingServicePrivate *d;
};

#endif // CUPOLLINGSERVICE_H
