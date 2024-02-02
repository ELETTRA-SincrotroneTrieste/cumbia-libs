#include "eabstractgauge.h"
#include <QPolygonF>
#include <elettracolors.h>
#include <cumacros.h>
#include <eng_notation.h>
#include <math.h>

#include <QtDebug>

EAbstractGauge::EAbstractGauge(QWidget *parent) : QWidget(parent),
	m_colorMode(COLORBAR),
	m_referenceEnabled(true), m_scaleEnabled(true),
	m_numMajorTicks(LINEAR_MAJOR_TICKS), longestLabelIndex(-1),
	m_normalColor(EColor(Elettra::green)), m_warningColor(EColor(Elettra::orange)), m_errorColor(EColor(Elettra::red)),
	d_logarithmicScale(false)
{
	m_minValue = -15;
	m_maxValue = 15;
	m_lowError = -10;
	m_highError = 10;
	m_lowWarning = -5;
	m_highWarning = 5;
	m_reference = 3;
	m_value = 0;
	d_naturalMinV = m_minValue;
	d_naturalMaxV = m_maxValue;
	d_naturalLowWarn = m_lowWarning;
	d_naturalHighWarn = m_highWarning;

	d_naturalLowErr = m_lowError;
	d_naturalHighErr = m_highError;
	d_naturalValue = m_value;
	d_naturalReference = m_reference;
	
	m_valueFormat = "%.1f";
	
	configure();
}

double EAbstractGauge::logarithm(double v)
{
  double logar;
  if(v > 0)
	logar = log10(v);
  else 
  {
	perr("Gauge \"%s\": logarithmic scale but value %.2f <= 0", qstoc(objectName()), v);
	logar = 0;
  }
  return logar;
}

void EAbstractGauge::setLogarithmicScale(bool en)
{
  d_logarithmicScale = en;
  /* high and low warn */
  d_logarithmicScale ?  m_lowWarning = logarithm(d_naturalLowWarn) : m_lowWarning = d_naturalLowWarn;
  d_logarithmicScale ?  m_highWarning = logarithm(d_naturalHighWarn) : m_highWarning = d_naturalHighWarn;
  /* errors */
  d_logarithmicScale ?  m_lowError = logarithm(d_naturalLowErr) : m_lowError = d_naturalLowErr;
  d_logarithmicScale ?  m_highError = logarithm(d_naturalHighErr) : m_highError = d_naturalHighErr;
  /* min and max value */
  d_logarithmicScale ?  m_minValue = logarithm(d_naturalMinV) : m_minValue = d_naturalMinV;
  d_logarithmicScale ?  m_maxValue = logarithm(d_naturalMaxV) : m_maxValue = d_naturalMaxV;
  /* value */
  d_logarithmicScale ? m_value = logarithm(d_naturalValue) : m_value = d_naturalValue;
  /* reference */
  d_logarithmicScale ? m_reference = logarithm(d_naturalReference) : m_reference = d_naturalReference;
  configure();
  update();
}

void EAbstractGauge::setLowWarning(double w)
{
	d_naturalLowWarn = w;
	if(d_logarithmicScale)
	  m_lowWarning = logarithm(w);
	else
	  m_lowWarning = w; 
	configure();
	update(); 
}

void EAbstractGauge::setHighWarning(double w)
{
	d_naturalHighWarn = w;
	if(d_logarithmicScale)
	  m_highWarning = logarithm(w);
	else
	  m_highWarning = w; 

	configure();
	update(); 
}

void EAbstractGauge::setLowError(double e)
{ 
	d_naturalLowErr = e;
	if(d_logarithmicScale)
	  m_lowError = logarithm(e);
	else
	  m_lowError = e; 

	configure(); 
	update(); 
}

void EAbstractGauge::setHighError(double e)
{ 
	d_naturalHighErr = e;
	if(d_logarithmicScale)
	  m_highError = logarithm(e);
	else
	  m_highError = e; 

	configure(); 
	update(); 
}

void EAbstractGauge::setMinValue(double v)
{ 
  d_naturalMinV = v;
  if(d_logarithmicScale)
	m_minValue = logarithm(v);
  else
	m_minValue = v; 
  configure(); 
  update(); 
}

void EAbstractGauge::setMaxValue(double v)
{ 
  d_naturalMaxV = v;
   if(d_logarithmicScale)
	m_maxValue = logarithm(v);
  else
	m_maxValue = v; 
  configure(); 
  update(); 
}

void EAbstractGauge::setReference(double r)
{ 
	d_naturalReference = r;
	if(d_logarithmicScale)
	  m_reference = logarithm(r);
	else
	  m_reference = r; 
	update(); 
}

void EAbstractGauge::setScaleEnabled(bool b)
{ 
	m_scaleEnabled = b; 
	configure(); 
	update(); 
}

void EAbstractGauge::setReferenceEnabled(bool b)
{ 
	m_referenceEnabled = b; 
	configure(); 
	update(); 
}

void EAbstractGauge::setColorMode(ColorMode m)
{ 
	m_colorMode = m; 
	configure(); 
	update(); 
}

void EAbstractGauge::setNormalColor(QColor c)
{ 
	m_normalColor = c; 
	configure(); 
	update(); 
}

void EAbstractGauge::setWarningColor(QColor c)
{ 
	m_warningColor = c; 
	configure(); 
	update(); 
}

void EAbstractGauge::setErrorColor(QColor c)
{ 
	m_errorColor = c; 
	configure(); 
	update(); 
}

void EAbstractGauge::setExternalScale(bool b)
{ 
	m_externalScale = b; 
	configure(); 
	update(); 
}

void EAbstractGauge::setNumMajorTicks(int t)
{ 
	m_numMajorTicks = t; 
	configure(); 
}

void EAbstractGauge::setNumMinorTicks(int t)
{ 
	m_numMinorTicks = t;
	configure(); 
}

void EAbstractGauge::configure()
{ 
	qreal interval = m_maxValue - m_minValue;
	v_p.clear();
	v_c.clear();
	if (m_colorMode == GRADIENT)
	{
		v_p << m_minValue << (m_minValue+m_lowError)*.5;
		v_p << (m_lowError+m_lowWarning)*.5;
		v_p << (m_lowWarning+m_highWarning)*.5 << (m_highWarning+m_highError)*.5;
		v_p << (m_highError+m_maxValue)*.5 << m_maxValue;
		v_c << m_errorColor << m_errorColor << m_warningColor << m_normalColor;
		v_c << m_warningColor << m_errorColor << m_errorColor;
		if ((m_minValue == m_lowError) && (m_lowError == m_lowWarning))
			v_c[0] = v_c[1] = v_c[2] = m_normalColor;
		if ((m_maxValue == m_highError) && (m_highError == m_highWarning))
			v_c[4] = v_c[5] = v_c[6] = m_normalColor;
	}
	else if (m_colorMode == COLORBAR)
	{
		v_p << m_minValue << m_lowError << m_lowWarning;
		v_c << m_errorColor    << m_errorColor    << m_warningColor;
		v_p << m_highWarning << m_highError << m_maxValue;
		v_c << m_normalColor     << m_warningColor  << m_errorColor;
	}
	else
	{
		if (m_value < m_lowError)
			v_c << m_errorColor;
		else if (m_value < m_lowWarning)
			v_c << m_warningColor;
		else if (m_value < m_highWarning)
			v_c << m_normalColor;
		else if (m_value < m_highError)
			v_c << m_warningColor;
		else 
			v_c << m_errorColor;
	}

	for (int i = 0; i < v_p.size(); i++)
		v_p[i] = (v_p[i] - m_minValue) / interval;

	labels.clear();
	double val = m_minValue;
	double tick = (m_maxValue-m_minValue)/((m_numMajorTicks-1)*m_numMinorTicks);
	double step = qRound((m_maxValue-m_minValue)/(m_numMajorTicks-1)/tick)*tick;
	longestLabelIndex = -1;
	for (int i = 0; i < m_numMajorTicks; i++)
	{
		double representedValue = val;
//#ifdef __FreeBSD__
//		d_logarithmicScale ? representedValue  = pow(val, 10) : representedValue  = val;
//#else
                d_logarithmicScale ? representedValue  = pow(val, 10) : representedValue  = val;
//#endif
                EngString engString(QString::asprintf(qstoc(m_valueFormat), representedValue), m_valueFormat, representedValue);
// 		QString s = QString::number(representedValue);
		if ((longestLabelIndex == -1) || engString.length() > labels[longestLabelIndex].length())
			longestLabelIndex = i;
		labels << engString;
		val += step;
	}
}

void EAbstractGauge::setValue(double v)
{ 
  /* v is a natural value, not logarithmic */
	if(v == d_naturalValue)
		return;
	
	d_naturalValue = v;
	d_logarithmicScale ? m_value = logarithm(v) : m_value = v;
	
	m_value = qBound(m_minValue, m_value, m_maxValue); 
	
	if (m_colorMode == SINGLECOLOR)
	{
		v_c.clear();
		if (m_value < m_lowError)
			v_c << m_errorColor;
		else if (m_value < m_lowWarning)
			v_c << m_warningColor;
		else if (m_value < m_highWarning)
			v_c << m_normalColor;
		else if (m_value < m_highError)
			v_c << m_warningColor;
		else 
			v_c << m_errorColor;
	}
	update();
}

void EAbstractGauge::setValue(int v)
{
	setValue((double)v);
}
