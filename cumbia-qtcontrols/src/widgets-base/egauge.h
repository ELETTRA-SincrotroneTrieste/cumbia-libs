#ifndef _EGAUGE_H
#define _EGAUGE_H

#include "eabstractgauge.h"
#include <math.h>
#include <QWidget>

#define LINEAR_MAJOR_TICKS 6
#define CIRCULAR_MAJOR_TICKS 11
#define NUM_MINOR_TICKS 5

#define DEPRECATED(func) func __attribute__ ((deprecated))

class QRect;
class QLineF;
class EAbstractGaugePrivate;
class ELinearGaugePrivate;
class ECircularGaugePrivate;


/**
 * \brief a thermometer like widget
 */
class ELinearGauge : public EAbstractGauge
{
Q_OBJECT

Q_ENUMS(FillMode)
	
Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation)
Q_PROPERTY(FillMode fillMode READ fillMode WRITE setFillMode)

public:
	/**
	 * How the value is displayed on the bar
	 */
	enum FillMode
	{
		ALL, /** fill the whole bar, moving marker */
		FROM_MIN, /** bar filled from min value to value */
		FROM_ZERO, /** bar filled from zero to value */
		FROM_REF /** bar filled from reference to value */
	};
	
	ELinearGauge(QWidget * = NULL, Qt::Orientation=Qt::Vertical);
	~ELinearGauge(){};

	void setOrientation(Qt::Orientation o){ m_orientation = o; configure(); update(); };
	Qt::Orientation orientation(){ return m_orientation; };

	void setFillMode(FillMode m){ m_fillMode = m; configure(); update(); };
	FillMode fillMode(){ return m_fillMode; };

protected:
	void paintEvent(QPaintEvent *);
	void drawColorBar(QPainter *);
	void drawScale(QPainter *);
	void drawMarker(QPainter *, bool);
	void drawLabels(QPainter *);
	virtual void configure();

private:
	Qt::Orientation	m_orientation;
	FillMode m_fillMode;
	int scalePos, labelsPos, totalSize;
	int barSize, scaleSize, labelsSize;

    ELinearGaugePrivate *d_ptr;
};

/**
 * \brief a tachometer like widget
 * 
 * A tachometer like widget.
 */
class ECircularGauge : public EAbstractGauge
{
Q_OBJECT

Q_PROPERTY(bool valueDisplayed READ isValueDisplayed WRITE setValueDisplayed)
Q_PROPERTY(QString label READ label WRITE setLabel)

public:
	ECircularGauge(QWidget * = NULL);
	~ECircularGauge(){};

	void setValueDisplayed(bool b){ m_valueDisplayed = b; update(); };
	bool isValueDisplayed(){ return m_valueDisplayed; };

	void setLabel(QString l){ m_label = l; update(); };
	QString label(){ return m_label; };
	
protected:
	void paintEvent(QPaintEvent *);
	void drawColorBar(QPainter *);
	void drawScale(QPainter *);
	void drawNeedle(QPainter *);
	void drawLabels(QPainter *);
	void drawValue(QPainter *);
	virtual void configure();
	
private:
	double m_startAngle, m_stopAngle, m_arcLength;
	int m_innerRadius, m_outerRadius, m_labelRadius;
	QString m_label;
	bool m_valueDisplayed;

    ECircularGaugePrivate *d_ptr;
};
#endif
