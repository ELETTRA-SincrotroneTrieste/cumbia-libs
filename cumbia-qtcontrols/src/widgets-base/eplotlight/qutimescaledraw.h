#ifndef QUTIMESCALEDRAW_H
#define QUTIMESCALEDRAW_H

#include <qwt_date_scale_draw.h>

/**
 * \brief QuPlot component to provide a date/time based scale.
 *
 * Derives from QwtDateScaleDraw. Date can be displayed or not
 * according to the dateEnabled property.
 *
 * \ingroup plots
 */
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
