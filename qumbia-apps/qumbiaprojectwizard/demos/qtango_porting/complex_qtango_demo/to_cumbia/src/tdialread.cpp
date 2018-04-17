#include "tdialread.h"
#include <cucontrolsreader_abs.h>
#include <cucontext.h>

TDialRead::TDialRead(QWidget *w, Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac) : QDial(w) {
    context = new CuContext(cumbia, r_fac);
}

CuContext *TDialRead::getContext() const {
    return context;
}

void TDialRead::onUpdate(const CuData &data){
    bool read_ok = !data["err"].toBool();
    if(read_ok) {
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
            data["value"].to<int>(v);
            setValue(v);
        }
    }
    setToolTip(QString::fromStdString(data["msg"].toString()));
    setEnabled(read_ok);
}

QString TDialRead::source() const
{
    if(CuControlsReaderA* r = context->getReader())
        return r->source();
    return "";
}

void TDialRead::setSource(const QString &s)
{
    CuControlsReaderA * r = context->replace_reader(s.toStdString(), this);
    if(r)
        r->setSource(s);
}
