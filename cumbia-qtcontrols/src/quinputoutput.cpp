#include "quinputoutput.h"
#include <qulabel.h>
#include <cudata.h>
#include <cumacros.h>
#include <cuvariant.h>
#include <enumeric.h>
#include <elineedit.h>
#include <qubutton.h>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QtDebug>
#include <QComboBox>
#include <cucontext.h>

class QuInputOutputPrivate
{
public:
    bool auto_configure;
    bool read_ok;
    QuInputOutput::WriterType w_type;
};

QuInputOutput::QuInputOutput(QWidget *parent, Cumbia* cumbia,
                             const CuControlsReaderFactoryI& r_fac,
                             const CuControlsWriterFactoryI &w_fac)
    : EInputOutputWidget(parent)
{
    d = new QuInputOutputPrivate;
    d->w_type = None;
    CuControlsReaderFactoryI *rf = r_fac.clone();
    // we need the
    CuData options;
    std::vector<std::string> props;
    props.push_back("values");
    options["fetch_props"] = props;
    rf->setOptions(options);
    QuLabel *label = new QuLabel(this, cumbia, *rf);
    setOutputWidget(label);
    connect(label, SIGNAL(newData(const CuData&)), this, SLOT(onNewData(const CuData&)));
    // Apply button
    new QuButton(getContainer(), cumbia, w_fac, "OK");
    delete rf;
}

QuInputOutput::QuInputOutput(QWidget *parent, CumbiaPool *cu_poo, const CuControlsFactoryPool &f_poo)
    : EInputOutputWidget(parent)
{
    d = new QuInputOutputPrivate;
    d->w_type = None;
    QuLabel *label = new QuLabel(this, cu_poo, f_poo);
    setOutputWidget(label);
    connect(label, SIGNAL(newData(const CuData&)), this, SLOT(onNewData(const CuData&)));
    // Apply button
    new QuButton(getContainer(), cu_poo, f_poo);
}

void QuInputOutput::m_init()
{

}

QuInputOutput::~QuInputOutput()
{
    delete d;
}

/** \brief sets the writer type to t
 *
 */
void QuInputOutput::setWriterType(QuInputOutput::WriterType t)
{
    if(t != d->w_type)
    {
        QWidget *in = inputWidget();
        if(in)
            delete in;
        switch(t)
        {
        case Numeric:
            setInputWidget(new ENumeric(this));
            break;
        case SpinBox:
            setInputWidget(new QSpinBox(this));
            break;
        case DoubleSpinBox:
            setInputWidget(new QDoubleSpinBox(this));
            break;
        case LineEdit:
            setInputWidget(new ELineEdit(this));
            break;
        case ComboBox:
            setInputWidget(new QComboBox(this));
            break;
        default:
            break;
        }
        d->w_type = t;
    }
}

QPushButton *QuInputOutput::getApplyButton()
{
    return findChild<QuButton *>();
}

QString QuInputOutput::source() const
{
    return outputWidget()->property("source").toString();
}

/** \brief Returns the context of the output widget
 *
 * @return a reference to the QuContext of the output widget.
 *
 * \note The current implementation works only if the reader is a QuLabel.
 */
CuContext *QuInputOutput::getOutputContext() const
{
    QuLabel *l = qobject_cast<QuLabel *>(outputWidget());
    if(l)
        return l->getContext();
    return NULL;
}

/** \brief Returns the context of the input widget.
 *
 * \note The current implementation returns NULL
 */
CuContext *QuInputOutput::getInputContext() const
{
    return NULL;
}

void QuInputOutput::setObjectName(const QString &name)
{
    EInputOutputWidget::setObjectName(name);
    if(inputWidget())
        inputWidget()->setObjectName(name + "_inputWidget");
    if(outputWidget())
        outputWidget()->setObjectName(name + "_outputWidget");
}

void QuInputOutput::setSource(const QString &s)
{
    QWidget *w = outputWidget();
    bool res = w->metaObject()->indexOfProperty("source") > -1;
    if(res)
        res = w->setProperty("source", s);
    if(!res)
        perr("QuInputOutput: property source is not available in %s (class: %s)", qstoc(objectName()),
             w->metaObject()->className());

    // there is (or there will be) an inputWidget providing arguments for the QuButton
    findChild<QuButton *>()->setTargets(s + "(&inputWidget)");
}

void QuInputOutput::unsetSource()
{
    QWidget *w = outputWidget();
    bool res = w->metaObject()->indexOfSlot(w->metaObject()->normalizedSignature("unsetSource()")) > -1;
    if(res)
        res = w->metaObject()->invokeMethod(w, "unsetSource");
    if(!res)
        perr("QuInputOutput: method unsetSource is not available in %s (class: %s)", qstoc(objectName()),
             w->metaObject()->className());
}

void QuInputOutput::onNewData(const CuData &da)
{
    if(!da["err"].toBool() && da["type"].toString() == "property")
    {
        m_configure(da);
        disconnect(sender(), SIGNAL(newData(const CuData&)));
    }
}

void QuInputOutput::m_configure(const CuData &da)
{
    std::string target = da["src"].toString();
    if(da["data_format_str"].toString() == "scalar" && da["writable"].toInt() > 0)
    {
        CuVariant v = da["value"];
        if(d->w_type == None && da["values"].isValid())
            setWriterType(ComboBox);
        else if(d->w_type == None && (v.isFloatingPoint()  || v.isInteger()) )
            setWriterType(Numeric);
        else if(d->w_type == None && v.getType() == CuVariant::String)
            setWriterType(LineEdit);

        if(d->w_type == ComboBox && da["values"].isValid())
        {
            QComboBox *c = static_cast<QComboBox* >(inputWidget());
            c->clear();
            c->setCurrentText(QString::fromStdString(v.toString()));
            if(da["values"].isValid())
            {
                std::vector<std::string> values = da["values"].toStringVector();
                for(size_t i = 0; i < values.size(); i++)
                    c->insertItem(i, QString::fromStdString(values[i]));
                if(v.isInteger())
                    c->setCurrentIndex(QString::fromStdString(v.toString()).toInt());
            }
        }
        else if(d->w_type == LineEdit)
        {
            static_cast<QLineEdit *>(inputWidget())->setText(QString::fromStdString(v.toString()));
        }
        else
        {
            QString desc;
            // get minimum and maximum from properties, if possible
            CuVariant m, M;
            m = da["min"];
            M = da["max"];
            std::string print_format = da["format"].toString();
            double min, max;
            bool ok;
            ok = m.to<double>(min);
            if(ok)
                ok = M.to<double>(max);
            QWidget *in = inputWidget();
            if(ok && d->w_type == Numeric)
            {
                ENumeric * en = static_cast<ENumeric *>(in);
                en->configureNumber(min, min, QString::fromStdString(print_format));
                /* integerDigits() and decimalDigits() from NumberDelegate */
                en->setIntDigits(en->integerDigits());
                en->setDecDigits(en->decimalDigits());
            }

            // the following applies to ENumeric or spin boxes
            if(ok)
            {
                in->setProperty("maximum", max);
                in->setProperty("miniumum", min);
                desc = "\n(min: "+ QString("%1").arg(min) + " max: "+ QString("%1").arg(max) + ")";
            }
            else
                pinfo("QuInputOutput: maximum and minimum values not set on the target \"%s\", object \"%s\": "
                      "not setting format nor maximum/minimum", target.c_str(), qstoc(objectName()));
            /* can set current values instead */
            double val;
            bool can_be_double = da["w_value"].to<double>(val);
            if (can_be_double)
                in->setProperty("value", val);
            if(!da["description"].isNull()) {
                desc.prepend(QString::fromStdString(da["description"].toString()));
            }
            in->setWhatsThis(desc);
        }
    }
    else
        perr("QuInputOutput [%s]: invalid data format \"%s\" or read only source \"%s\" (writable: %d)", qstoc(objectName()),
            da["data_format_str"].toString().c_str(), target.c_str(), da["writable"].toInt());

    if(inputWidget())
        inputWidget()->setObjectName("inputWidget");

    qDebug() << __FUNCTION__ << findChild<QWidget *>("inputWidget");
}
