#include "qutimescaledraw.h"
#include <qwt_date.h>

QuTimeScaleDraw::QuTimeScaleDraw(Qt::TimeSpec timeSpec)
    : QwtDateScaleDraw(timeSpec)
{
    setDateEnabled(false);
}

void QuTimeScaleDraw::setDateEnabled(bool en)
{
    if(en)
    {
        setDateFormat( QwtDate::Millisecond, "MMM.dd hh:mm:ss:zzz" );
        setDateFormat( QwtDate::Second, "MMM.dd hh:mm:ss" );
        setDateFormat( QwtDate::Minute, "MMM.dd hh:mm" );
        setDateFormat( QwtDate::Hour, "MMM.dd hh:mm" );
        setDateFormat( QwtDate::Day, "ddd dd MMM" );
        setDateFormat( QwtDate::Week, "Www" );
        setDateFormat( QwtDate::Month, "MMM" );
    }
    else
    {
        setDateFormat( QwtDate::Millisecond, "hh:mm:ss:zzz" );
        setDateFormat( QwtDate::Second, "hh:mm:ss" );
        setDateFormat( QwtDate::Minute, "hh:mm" );
        setDateFormat( QwtDate::Hour, "hh:mm" );
        setDateFormat( QwtDate::Day, "ddd dd MMM" );
        setDateFormat( QwtDate::Week, "Www" );
        setDateFormat( QwtDate::Month, "MMM" );
    }
    m_dateEnabled = en;
}

bool QuTimeScaleDraw::dateEnabled() const { return m_dateEnabled; }
