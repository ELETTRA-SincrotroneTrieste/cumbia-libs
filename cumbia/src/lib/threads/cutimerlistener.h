#ifndef CUTIMERLISTENER_H
#define CUTIMERLISTENER_H

class CuTimer;

/*! @private
 *
 */
class CuTimerListener
{
public:

    virtual ~CuTimerListener() {}

    virtual void onTimeout(CuTimer*) = 0;
};

#endif // CUTIMERLISTENER_H
