#ifndef QUTIMESCALEDRAW_H
#define QUTIMESCALEDRAW_H

#include <qwt_date_scale_draw.h>

class QuTimeScaleDraw : public QwtDateScaleDraw
{
public:
    QuTimeScaleDraw(Qt::TimeSpec timeSpec= Qt::LocalTime);

    void setDateEnabled(bool en);

    bool dateEnabled() const;

private:
    bool m_dateEnabled;
};

#endif // QUTIMESCALEDRAW_H
