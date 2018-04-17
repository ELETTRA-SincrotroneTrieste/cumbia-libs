#include "tdialread.h"

TDialRead::TDialRead(QWidget *parent) : QDial(parent),
    QTangoComProxyReader(this),
    QTangoWidgetCommon(this)
{
    connect(qtangoComHandle(), SIGNAL(attributeAutoConfigured(const TangoConfigurationParameters*)),
            this, SLOT(autoConfigure(const TangoConfigurationParameters* )));
    connect(qtangoComHandle(), SIGNAL(newData(TVariant)), this, SLOT(refresh(TVariant)));
   QTangoWidgetCommon::setDragEnabled(false);
}

void TDialRead::refresh(const TVariant &v)
{
    setEnabled(v.quality() != Tango::ATTR_INVALID);
    setToolTip(v.message());
    if(v.canConvertToInt())
        setValue(v.toInt());
    else
        setToolTip(toolTip() + "\n\nWrong data type");
}

void TDialRead::autoConfigure(const TangoConfigurationParameters *cp)
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
        setValue(cp->currentValue().toInt());
}

