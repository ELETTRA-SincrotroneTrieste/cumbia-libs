#include "qucheckbox.h"

#include <QCheckBox>
#include <cudata.h>
#include <cumacros.h>
#include <cumbiapool.h>
#include <cuvariant.h>
#include <cucontext.h>
#include <QtDebug>

#include "cucontrolsreader_abs.h"
#include "cucontrolswriter_abs.h"
#include "cucontrolsfactories_i.h"
#include "cucontrolsfactorypool.h"
#include "culinkstats.h"
#include "cucontextmenu.h"

class QuCheckboxPrivate
{
public:
    bool auto_configure;
    bool read_ok, last_val;
    bool text_from_label;
    CuContext *in_ctx, *out_ctx;
};

QuCheckBox::QuCheckBox(QWidget *parent, Cumbia *cumbia,
                       const CuControlsReaderFactoryI &r_fac,
                       const CuControlsWriterFactoryI &w_fac)
    : QCheckBox(parent)
{
    d = new QuCheckboxPrivate;
    m_init();
    d->out_ctx = new CuContext(cumbia, r_fac);
    d->in_ctx = new CuContext(cumbia, w_fac);
}

QuCheckBox::QuCheckBox(QWidget *parent, CumbiaPool *cu_poo, const CuControlsFactoryPool &f_poo)
    : QCheckBox(parent)
{
    d = new QuCheckboxPrivate;
    m_init();
    d->out_ctx = new CuContext(cu_poo, f_poo);
    d->in_ctx = new CuContext(cu_poo, f_poo);
}

QuCheckBox::~QuCheckBox()
{
    delete d->in_ctx;
    delete d->out_ctx;
    delete d;
}

void QuCheckBox::m_init()
{
    d->in_ctx = d->out_ctx = NULL;
    d->auto_configure = true;
    d->read_ok = false;
    d->text_from_label = true;
    setText("No Link");
    connect(this, SIGNAL(clicked()), this, SLOT(checkboxClicked()));
}

QString QuCheckBox::source() const
{
    if(CuControlsReaderA* r = d->out_ctx->getReader())
        return r->source();
    return "";
}

/** \brief Connect the reader and the writer to the specified source name.
 *
 * If a reader with a different source is configured, it is deleted.
 * The same goes for the writer.
 * If options have been set with QuContext::setOptions, they are used to set up the reader as desired.
 *
 * \note the reader and the writer are connected to the same source.
 *
 * @see QuContext::setOptions
 * @see source
 */
void QuCheckBox::setSource(const QString &s)
{
    CuControlsReaderA * r = d->out_ctx->replace_reader(s.toStdString(), this);
    if(r)
        r->setSource(s);
    CuControlsWriterA *w = d->in_ctx->replace_writer(s.toStdString(), this);
    if(w)
        w->setTargets(s);
}

void QuCheckBox::setTextFromLabel(bool tfl)
{
    d->text_from_label = tfl;
    if(tfl && property("label").isValid())
        setText(property("label").toString());
}

void QuCheckBox::contextMenuEvent(QContextMenuEvent *e)
{
    CuContextMenu* m = new CuContextMenu(this, this);
    connect(m, SIGNAL(linkStatsTriggered(QWidget*, CuContextI *)),
            this, SIGNAL(linkStatsRequest(QWidget*, CuContextI *)));
    m->popup(e->globalPos());
}

void QuCheckBox::checkboxClicked()
{
      CuVariant arg(d->last_val);
      CuControlsWriterA *w = d->in_ctx->getWriter();
      if(w) {
          w->setArgs(arg);
          w->execute();
      }
}

/** \brief Provided to implement CuContextI interface, returns getOutputContext
 *
 * @return the output widget's context
 *
 * @see getOutputContext
 */
CuContext *QuCheckBox::getContext() const
{
    return d->out_ctx;
}

/** \brief Returns the context of the output widget
 *
 * @return a reference to the QuContext of the output widget.
 *
 * \note The current implementation works only if the reader is a QuLabel.
 */
CuContext *QuCheckBox::getOutputContext() const
{
    return d->out_ctx;
    return NULL;
}

CuContext *QuCheckBox::getInputContext() const
{
    return d->in_ctx;
}

bool QuCheckBox::textFromLabel() const
{
    return d->text_from_label;
}

void QuCheckBox::onUpdate(const CuData &da)
{
    d->read_ok = !da["err"].toBool();
    if(d->read_ok && d->auto_configure && da["type"].toString() == "property" &&
            da.containsKey("label")) {
        setProperty("label", QString::fromStdString(da["label"].toString()) );
        if(d->text_from_label)
            setText(property("label").toString());
    }
    setEnabled(d->read_ok);
    // update link statistics
    d->out_ctx->getLinkStats()->addOperation();
    // tooltip message
    setToolTip(da["msg"].toString().c_str());

    if(!d->read_ok)
        d->out_ctx->getLinkStats()->addError(da["msg"].toString());
    else if(da.containsKey("value"))
    {
        CuVariant val = da["value"];
        if(val.getType() == CuVariant::Boolean) {
            d->last_val = val.toBool();
            d->last_val ? setChecked(Qt::Checked) : setChecked(Qt::Unchecked);
        }
        else {
            setChecked(Qt::PartiallyChecked);
            setToolTip(QString("wrong data type [%1]").arg(QString::fromStdString(val.dataTypeStr(val.getType()))));
        }
    }
}

