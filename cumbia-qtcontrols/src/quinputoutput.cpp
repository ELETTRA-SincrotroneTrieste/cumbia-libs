#include "quapplication.h"
#include "quinputoutput.h"
#include <qulabel.h>
#include <cudata.h>
#include <cumacros.h>
#include <cuvariant.h>
#include <enumeric.h>
#include <qubutton.h>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QtDebug>
#include <QComboBox>
#include <QLineEdit>
#include <cucontext.h>
#include <qustring.h>
#include <cumbiapool.h>

class QuInputOutputPrivate
{
public:
    QuInputOutputPrivate() : auto_configure(true), index_mode(false), index_offset(0), w_type(QuInputOutput::None) {}
    bool auto_configure;
    bool read_ok, index_mode;
    int index_offset;
    QuInputOutput::WriterType w_type;
};

/** \brief Constructor with the parent widget, an *engine specific* Cumbia implementation and a CuControlsReaderFactoryI interface.
 *
 *  Please refer to \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 */
QuInputOutput::QuInputOutput(QWidget *parent, Cumbia* cumbia,
                             const CuControlsReaderFactoryI& r_fac,
                             const CuControlsWriterFactoryI &w_fac)
    : EInputOutputWidget(parent)
{
    d = new QuInputOutputPrivate;
    CuControlsReaderFactoryI *rf = r_fac.clone();
    // we need the
    CuData options;
    std::vector<std::string> props;
    props.push_back("values");
    options["fetch_props"] = props;
    rf->setOptions(options);
    QuLabel *label = new QuLabel(this, cumbia, *rf);
    label->setDrawInternalBorder(false);
    setOutputWidget(label);
    connect(label, SIGNAL(newData(const CuData&)), this, SLOT(onNewData(const CuData&)));
    // Apply button
    new QuButton(getContainer(), cumbia, w_fac, "OK");
    delete rf;
}

/** \brief Constructor with the parent widget, *CumbiaPool*  and *CuControlsFactoryPool*
 *
 *   Please refer to \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 */
QuInputOutput::QuInputOutput(QWidget *parent, CumbiaPool *cu_poo, const CuControlsFactoryPool &f_poo)
    : EInputOutputWidget(parent)
{
    d = new QuInputOutputPrivate;
    QuLabel *label = new QuLabel(this, cu_poo, f_poo);
    label->setDrawInternalBorder(false);
    label->getContext()->setOptions(CuData("fetch_props", std::vector<std::string>{ "props" }));
    setOutputWidget(label);
    connect(label, SIGNAL(newData(const CuData&)), this, SLOT(onNewData(const CuData&)));
    // Apply button
    new QuButton(getContainer(), cu_poo, f_poo);
}

QuInputOutput::QuInputOutput(QWidget *parent)
    : EInputOutputWidget(parent) {
    d = new QuInputOutputPrivate;
    QuApplication *a = static_cast<QuApplication *>(QCoreApplication::instance());
    QuLabel *label = new QuLabel(this, a->cumbiaPool(), *a->fpool());
    label->setDrawInternalBorder(false);
    label->getContext()->setOptions(CuData("fetch_props", std::vector<std::string>{ "props" }));
    setOutputWidget(label);
    connect(label, SIGNAL(newData(const CuData&)), this, SLOT(onNewData(const CuData&)));
    // Apply button
    new QuButton(getContainer(), a->cumbiaPool(), *a->fpool());
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
 * @param t the writer type, one of the values of the QuInputOutput::WriterType enum
 *
 * The current input widget is destroyed and replaced if necessary.
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
        case ComboBox: {
            QComboBox *cb = new QComboBox(this);
            cb->setProperty("indexMode", d->index_mode); // default false
            cb->setProperty("indexOffset", d->index_offset); // default 0
            setInputWidget(cb);
        }
            break;
        case LineEdit:
        default:
            setInputWidget(new QLineEdit(this));
            break;
        }
        d->w_type = t;
    }
}

/** \brief returns the apply button (of type QuButton)
 *
 * @return the QuButton used as apply button
 */
QPushButton *QuInputOutput::getApplyButton()
{
    return findChild<QuButton *>();
}

/** \brief Returns the name of the link
 *
 * @return a string with the name of the link.
 *
 * \note This method returns the source property of the output widget
 *
 * See \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 */
QString QuInputOutput::source() const
{
    return outputWidget()->property("source").toString();
}

/** \brief Provided to implement CuContextI interface, returns getOutputContext
 *
 * @return the output widget's context
 *
 * @see getOutputContext
 */
CuContext *QuInputOutput::getContext() const {
    return getOutputContext();
}

/** \brief Returns the context of the output widget
 *
 * @return a pointer to the QuContext of the output widget.
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
 * @return a pointer to the QuContext of the input widget.
 */
CuContext *QuInputOutput::getInputContext() const
{
    return findChild<QuButton *>()->getContext();
}

/** \brief Set the object name and the name of the input and output widget as well.
 *
 * @param name the object name.
 *
 * If you set the object name, you will be able to find the input and output widgets
 * with the following names:
 *
 * \li name + "_inputWidget"
 * \li name + "_outputWidget"
 *
 */
void QuInputOutput::setObjectName(const QString &name)
{
    EInputOutputWidget::setObjectName(name);
    if(inputWidget())
        inputWidget()->setObjectName(name + "_inputWidget");
    if(outputWidget())
        outputWidget()->setObjectName(name + "_outputWidget");
}

bool QuInputOutput::indexMode() const {
    return d->index_mode;
}

int QuInputOutput::indexOffset() const {
    return d->index_offset;
}

/** \brief Link the reader/writer to the specified source.
 *
 * @param s the name of the source of the connection.
 *
 * \note The QuInputOutput reads and writes from *the same source* specified in s.
 *
 * Any cumbia widget with the *source* property can be used as output widget.
 *
 * See \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 *
 */
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
    findChild<QuButton *>()->setTarget(s + "(&inputWidget)");
}

/** \brief unlink the QuInputOutput
 *
 * The unsetSource method is invoked on the outputWidget.
 */
void QuInputOutput::unsetSource()
{
    QWidget *w = outputWidget();
    bool res = w->metaObject()->indexOfSlot(w->metaObject()->normalizedSignature("unsetSource()")) > -1;
    if(res)
        res = w->metaObject()->invokeMethod(w, "unsetSource");
    if(!res)
        perr("QuInputOutput: method unsetSource is not available in %s (class: %s)", qstoc(objectName()),
             w->metaObject()->className());
    findChild<QuButton *>()->clearTarget();
}

bool QuInputOutput::ctxSwap(CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool) {
    QWidget *w = outputWidget();
    bool res = w->metaObject()->indexOfProperty("source") > -1 && w->metaObject()->indexOfMethod("ctxSwap") >= 0;
    if(res)
        res = QMetaObject::invokeMethod(w, "ctxSwap", Q_RETURN_ARG(bool, res), Q_ARG(CumbiaPool*, cumbia_pool), Q_ARG(CuControlsFactoryPool, fpool));
    if(res)
        res = findChild<QuButton *>()->ctxSwap(cumbia_pool, fpool);
    return res;
}

void QuInputOutput::setIndexMode(bool m) {
    d->index_mode = m; // save if property set before input widget
    if(d->w_type == ComboBox && inputWidget()) // set if input widget created after property set
        inputWidget()->setProperty("indexMode", m);
}

void QuInputOutput::setIndexOffset(int o) {
    d->index_offset = o; // save if property set before input widget
    if(d->w_type == ComboBox && inputWidget()) // set if input widget created after property set
        inputWidget()->setProperty("indexOffset", o);
}

/// @private
void QuInputOutput::onNewData(const CuData &da) {
    if(!da[TTT::Err].toBool() && da[TTT::Type].toString() == "property")  // da["err"], da["type"]
    {
        m_configure(da);
        disconnect(sender(), SIGNAL(newData(const CuData&)));
    }
}

/// @private
void QuInputOutput::m_configure(const CuData &da) {
    std::string target = da[TTT::Src].toString();  // da["src"]
    if(da[TTT::DataFormatStr].toString() == "scalar" && da["writable"].toInt() > 0)  // da["dfs"]
    {
        CuVariant v = da[TTT::Value], wv = da[TTT::WriteValue];  // da["value"], da["w_value"]
        if(d->w_type == None && da["values"].isValid())
            setWriterType(ComboBox);
        else if(d->w_type == None && !v.isNull() && (v.isFloatingPoint()  || v.isInteger()) )
            setWriterType(Numeric);
        else if(d->w_type == None && !v.isNull() && v.getType() == CuVariant::String)
            setWriterType(LineEdit);

        if(d->w_type == ComboBox && da["values"].isValid()) {
            QComboBox *c = static_cast<QComboBox* >(inputWidget());
            c->clear();
            if(da["values"].isValid()) {
                bool imode = c->property("indexMode").toBool(); // combo index mode
                std::vector<std::string> values = da["values"].toStringVector();
                for(size_t i = 0; i < values.size(); i++)
                    c->insertItem(i, QString::fromStdString(values[i]));
                c->setCurrentIndex(imode ? QuString(wv.toString()).toInt() : c->findText(QuString(wv.toString())));
            }
        }
        else if(d->w_type == LineEdit) {
            static_cast<QLineEdit *>(inputWidget())->setText(QString::fromStdString(wv.toString()));
        }
        else {
            QString desc;
            // get minimum and maximum from properties, if possible
            CuVariant m, M;
            m = da[TTT::Min];  // da["min"]
            M = da[TTT::Max];  // da["max"]
            std::string print_format = da[TTT::NumberFormat].toString();  // da["format"]
            double min, max;
            bool ok;
            ok = m.to<double>(min);
            if(ok)
                ok = M.to<double>(max);
            QWidget *in = inputWidget();
            if(in && d->w_type == Numeric)
            {
                ENumeric * en = qobject_cast<ENumeric *>(in);
                if(!ok)
                    min = max = 0.0f;
                en->configureNumber(min, max, QString::fromStdString(print_format));
                /* integerDigits() and decimalDigits() from NumberDelegate */
                en->setIntDigits(en->integerDigits());
                en->setDecDigits(en->decimalDigits());
                if(ok) {
                    en->setMinimum(min);
                    en->setMaximum(max);
                }
                else {
                    en->updateMaxMinAuto();
                }
            }
            // the following applies to ENumeric or spin boxes
            else if(in && ok)
            {
                in->setProperty("maximum", max);
                in->setProperty("miniumum", min);
                desc = "\n(min: "+ QString("%1").arg(min) + " max: "+ QString("%1").arg(max) + ")";
            }
            else
                printf("QuInputOutput: maximum and minimum values not available for target \"%s\", object \"%s\": "
                      "using format %s to configure integer and decimal digits", target.c_str(), qstoc(objectName()), print_format.c_str());
            /* can set current values instead */
            double val;
            bool can_be_double = da[TTT::WriteValue].to<double>(val);  // da["w_value"]
            if (in && can_be_double)
                in->setProperty("value", val);
            if(!da[TTT::Description].isNull()) {  // da["description"]
                desc.prepend(QString::fromStdString(da[TTT::Description].toString()));  // da["description"]
            }
            if(in)
                in->setWhatsThis(desc);
        }
    }
    else
        perr("QuInputOutput [%s]: invalid data format \"%s\" or read only source \"%s\" (writable: %d)", qstoc(objectName()),
            da[TTT::DataFormatStr].toString().c_str(), target.c_str(), da["writable"].toInt());  // da["dfs"]

    if(inputWidget())
        inputWidget()->setObjectName("inputWidget");
}
