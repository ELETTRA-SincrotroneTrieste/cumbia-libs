#include "tdialwrite.h"
#include <cucontrolswriter_abs.h>
#include <cucontext.h>


TDialWrite::TDialWrite(QWidget *parent, Cumbia *cumbia, const CuControlsWriterFactoryI &w_fac)
    : QDial(parent)
{
    context = new CuContext(cumbia, w_fac);
    connect(this, SIGNAL(valueChanged(int)), this, SLOT(write(int)));
}

QString TDialWrite::target() const
{
    CuControlsWriterA *w = context->getWriter();
    if(w != NULL)
        return w->target();
    return "";
}

void TDialWrite::setTarget(const QString &targets)
{
    CuControlsWriterA* w = context->replace_writer(targets.toStdString(), this);
    if(w) w->setTarget(targets);
}

void TDialWrite::write(int val)
{
    CuVariant args(val);
    CuControlsWriterA *w = context->getWriter();
    if(w) {
        w->setArgs(args);
        w->execute();
    }
}

CuContext *TDialWrite::getContext() const {
    return context;
}

void TDialWrite::onUpdate(const CuData &data) {
    bool read_ok = !data["err"].toBool();
    bool format_ok = data["data_format_str"] == "scalar" && data["writable"].toInt() > 0;
    if(read_ok && format_ok) {
        QString desc;
        if(data["type"].toString() == "property") {
            // configure!
            double m, M;
            if(data["min"].to<double>(m) && data["max"].to<double>(M)) {
                setMinimum(m);
                setMaximum(M);
            }
            if(data["description"].toString().size())
                desc = QString::fromStdString(data["description"].toString());
            if(data["display_unit"].toString().size())
                desc += " [" + QString::fromStdString(data["display_unit"].toString()) + "]";
            if(!desc.isEmpty())
                emit description(desc);
        }
        else {
            int v;
            data["w_value"].to<int>(v);
            setValue(v);
        }
    }
    else if(!format_ok)
        setToolTip("the data format of " + QString::fromStdString(data["src"].toString()) + " is not scalar or not writable");
    else
        setToolTip(QString::fromStdString(data["msg"].toString()));

    setEnabled(read_ok && format_ok);
}
