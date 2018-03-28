#include <QtGui>

#include "tlabelitem.h"
#include <elettracolors.h>

TLabelItem::TLabelItem(QObject *p) : QObject(p), QTangoComProxyReader(this)//, app(0)
{
	m_stateColors			= QVector<QColor>(14, EColor(Elettra::darkYellow));
	m_stateColors[Tango::ON]	= EColor(Elettra::green);
	m_stateColors[Tango::FAULT]	= EColor(Elettra::red);
	m_stateColors[Tango::ALARM]	= EColor(Elettra::red);

	m_trueColor	=QColor(Qt::green);
	m_trueString	= "True";
	m_falseColor	=QColor(Qt::red);
	m_falseString	= "False";

	m_mode = Simple;
	ellipseRect = QRectF(0,0,10,10);

	setAcceptsHoverEvents(true);
	setFlags(QGraphicsItem::ItemIsMovable);

	connect(qtangoComHandle(), SIGNAL(newData(const TVariant&)), this, SLOT(refresh(const TVariant&)),
	  Qt::DirectConnection);
}

void TLabelItem::refresh(const TVariant& v)
{
	//setToolTip(QString::fromStdString(getDeviceName()));
	if (v.quality() == ATTR_INVALID)
	{
		brushColor =QColor(Qt::gray);
		setText("####");
	}
	else
	{
		if (v.canConvertToState())
		{
			Tango::DevState s = v.toState();
			brushColor = m_stateColors.at(s);
			setText(Config::instance()->stateString(s));
		}
		else if (v.canConvertToBool())
		{
			if (v.toBool())
			{
				brushColor = m_trueColor;
				setText(m_trueString);
			}
			else
			{
				brushColor = m_falseColor;
				setText(m_falseString);
			}
		}
		else if (v.canConvertToString())
		{
			brushColor =QColor(Qt::white);
			setText(v.toString());
		}
	}
}

void TLabelItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	painter->setPen(QColor(Qt::black));
	painter->setBrush(brushColor);
	
	QRectF r = boundingRect();
	if (m_mode == Simple)
	{
		painter->drawEllipse(ellipseRect);
	}
	else
	{
		painter->drawRect(r.x()-2, r.y()-1, r.width()+4, r.height()+2);
		QGraphicsSimpleTextItem::paint(painter, option, widget);
	}
}

/*void TLabelItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
	Q_UNUSED(event);
	if (helperApplication.isNull())
	{
		string a = TLoggingFactory::instance()->findHelperApplication(getDeviceName());
		if (a != "")
		{
			helperApplication = QString::fromStdString(a + " " + getDeviceName());
		}
		else
			helperApplication = QString("atkpanel ") +  QString::fromStdString(getDeviceName());
	}
	if (!helperApplication.isNull())
	{
		launchHelperApplication();
	}
}*/

/*void TLabelItem::launchHelperApplication()
{
	if (!app)
		app = new EApplicationLauncher(helperApplication);
	app->Rise();
}*/
    
void TLabelItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
	Q_UNUSED(event);
	if (ellipseRect.contains(event->pos()))
		m_mode = Zoomed;
	update();
}

void TLabelItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
	Q_UNUSED(event);
	if (ellipseRect.contains(event->pos()))
		m_mode = Zoomed;
	update();
}

void TLabelItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
	Q_UNUSED(event);
	m_mode = Simple;
	update();
}

/*QRectF TLabelItem::boundingRect()
{
	qDebug("TLabelItem::boundingRect");
	return QGraphicsSimpleTextItem::boundingRect();
}*/


