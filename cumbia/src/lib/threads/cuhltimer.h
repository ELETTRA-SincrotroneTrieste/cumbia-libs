#ifndef CUHLTIMER_H
#define CUHLTIMER_H

class Cumbia;
class CuHLTimer_P;

#include <cutimerlistener.h>


class CuHLTimerListener {
public:
    virtual void onTimeout(unsigned int period) = 0;
};

/*!
 * \brief The CuHLTimer class offers a higher level interface to CuTimer
 *
 * CuHLTimer timeouts are notified through a CuHLTimerListener
 * Notifications carry an integer value representing the *period* (*timeout*).
 * With this strategy, a single CuHLTimer can deliver timeouts at different periods,
 * configured with addTimerListener.
 *
 */
class CuHLTimer : public CuTimerListener
{
public:
    CuHLTimer(Cumbia *c, CuHLTimerListener *tl);
    CuHLTimer(Cumbia *c);
    virtual ~CuHLTimer();

    void start();
    void stop();
    bool isRunning() const;

    bool addTimerListener(CuHLTimerListener *tl, unsigned int timeout);
    void removeTimerListener(CuHLTimerListener *tl);

    bool checkTimeout(unsigned int t) const;

    // CuTimerListener interface
public:
    void onTimeout(CuTimer *t);

private:
    CuHLTimer_P *d;
    bool m_duplicate_pair(CuHLTimerListener *tl, unsigned int timeout) const;
};


#endif // CUHLTIMER_H
