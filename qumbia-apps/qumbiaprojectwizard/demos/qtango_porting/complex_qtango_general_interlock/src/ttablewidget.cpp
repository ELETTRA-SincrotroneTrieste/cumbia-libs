#include <QtDebug>

#include "ttablewidget.h"
#include <elettracolors.h>

TTableWidget::TTableWidget(QWidget *p) : QTableWidget(p), QTangoComProxyReader(this), QTangoWidgetCommon(this)
{
	setColumnCount(16);
	setAutoConfiguration(true);
	connect(qtangoComHandle(), SIGNAL(newData(const TVariant&)), this, SLOT(refresh(const TVariant&)),
	  Qt::DirectConnection);
	connect(qtangoComHandle(), SIGNAL(attributeAutoConfigured(const TangoConfigurationParameters *)), this, SLOT(configure(const 	TangoConfigurationParameters *)));
	setDesiredAttributePropertyList(QStringList() << "labels");
}

void TTableWidget::configure(const TangoConfigurationParameters *cp)
{
  if(cp->propertyFound("labels"))
  {
     _labels = cp->attributePropertyValue("labels");
     printf("\e[1;32m configuring labels on ttable\e[0m\n");
     qslisttoc(_labels);
     refresh(currentValue());
  }
  else
   perr("TTableWidget::configure(): source \"%s\" hasn't got the values property", qstoc(source()));
     
}

void TTableWidget::refresh(const TVariant& v)
{
   printf("\e[0;33mrefresh()\e[0m\n");
	if (v.quality() == ATTR_INVALID)
	{
		setDisabled(true);
	}
	else if (v.canConvertToBoolVector())
	{
		setDisabled(false);
		QVector<bool> data = v.toBoolVector();
		if (data == oldData)
			return;
		oldData = data;
		clear();
		setRowCount(data.size() / columnCount() + (data.size()%columnCount() ? 1 : 0));
		for (unsigned int i = 0; i < data.size(); i++)
		{
			QTableWidgetItem *item = new QTableWidgetItem();
			item->setFlags(Qt::ItemIsEnabled);
			item->setBackground(QBrush(EColor(data[i] ? Elettra::green : Elettra::red)));
			item->setText(_labels.value(i));
			item->setTextAlignment(Qt::AlignCenter);
			item->setToolTip(_descriptions.value(i));
			setItem(i / columnCount(), i % columnCount(), item);
			printf("\e[1;36msetting label \"%s\" ", qstoc(_labels.value(i)));
		}
		printf("\n\e[1;32m --- done refresh()\e[0m\n");
		resizeColumnsToContents();
	}
}


