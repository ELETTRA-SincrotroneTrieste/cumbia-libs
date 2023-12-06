#include "eapplynumeric.h"
#include <cumacros.h>
#include <QVBoxLayout>
#include <QHBoxLayout>

EApplyNumeric::EApplyNumeric(QWidget *parent, int i, int d, Qt::Orientation pos) : QWidget(parent), FloatDelegate()
{
	box = NULL;
	data = NULL;
	button = NULL;
	buttonPos = pos;
	intDig = i;
	decDig = d;
	d_applyButtonActive = true;
	init();
}

void EApplyNumeric::init()
{
	if (box)
		delete box;
		
	if (buttonPos == Qt::Horizontal)
		box = new QHBoxLayout(this);
	else if (buttonPos == Qt::Vertical)
		box = new QVBoxLayout(this);

    box->setContentsMargins(0,0,0,0);
	box->setSpacing(3);

	if (!data)
		data = new ENumeric(this, intDig, decDig);
	if (!button)
		button = new EApplyButton(this);
	box->addWidget(data, 3);
	box->addWidget(button, 1);

	connect(data, SIGNAL(valueChanged(double)), this, SLOT(numericValueChanged(double)));
        /* map ENumeric valueChanged() signal into EApplyNumeric omonimous signal */
        connect(data, SIGNAL(valueChanged(double)), this, SIGNAL(valueChanged(double)));
	connect(button, SIGNAL(clicked()), this, SLOT(applyValue()));
}

void EApplyNumeric::setFont(const QFont &f)
{
	QWidget::setFont(f);
	data->setFont(f);
	button->setFont(f);
}

void EApplyNumeric::applyValue()
{
  emit clicked(data->value());
  
}

bool EApplyNumeric::isModified()
{
  if(button)
	return button->isModified();
  return false;
}

QSize EApplyNumeric::sizeHint() const
{
    int w = button->sizeHint().width() + data->sizeHint().width();
    int h = data->sizeHint().height();
    return QSize(w, h);
}

QSize EApplyNumeric::minimumSizeHint() const
{
    int w = button->minimumSizeHint().width() + data->minimumSizeHint().width();
    int h = data->minimumSizeHint().height();
    return QSize(w, h);
}

void EApplyNumeric::updateMaxMinAuto() {
    data->updateMaxMinAuto();
}

void EApplyNumeric::numericValueChanged(double val)
{
  if(d_applyButtonActive && button)
        button->valueModified(val);
}



