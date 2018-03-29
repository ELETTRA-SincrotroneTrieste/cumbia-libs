#include "tdialwrite.h"

TDialWrite::TDialWrite(QWidget *parent) :
    QDial(parent),
    QTangoComProxyWriter(this),
    QTangoWidgetCommon(this)
{
    connect(qtangoComHandle(), SIGNAL(attributeAutoConfigured(const TangoConfigurationParameters*)),
            this, SLOT(autoConfigure(const TangoConfigurationParameters*)));
    connect(this, SIGNAL(valueChanged(int)), this, SLOT(write(int)));
    QTangoWidgetCommon::setDragEnabled(false);
}

void TDialWrite::autoConfigure(const TangoConfigurationParameters *cp)
{
    if(cp->minIsSet())
        setMinimum(cp->minValue());
    if(cp->maxIsSet())
        setMaximum(cp->maxValue());

    QString desc;
    if(cp->descriptionIsSet())
        desc = cp->description() + " ";
    if(cp->displayUnitIsSet())
        desc += "[" + cp->displayUnit() + "]";
    emit description(desc);

    if(cp->currentValue().canConvertToInt())
        setValue(cp->currentValue().toInt(false)); // false: get the write value
}

void TDialWrite::write(int val)
{
    QTangoComProxyWriter::execute(val);
}

