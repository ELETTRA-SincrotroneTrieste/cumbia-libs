#include "qucheckbox.h"

#include <QCheckBox>
#include <QContextMenuEvent>
#include <cudata.h>
#include <cumacros.h>
#include <cumbiapool.h>
#include <cuvariant.h>
#include <cucontext.h>
#include <QtDebug>
#include <cucontrolsutils.h>

#include "cucontrolsreader_abs.h"
#include "cucontrolswriter_abs.h"
#include "cucontrolsfactories_i.h"
#include "cucontrolsfactorypool.h"
#include "culinkstats.h"
#include "cucontextmenu.h"
#include "culog.h"
#include "qulogimpl.h"
#include "cuserviceprovider.h"

/// @private
class QuCheckboxPrivate
{
public:
    bool auto_configure;
    bool ok, last_val;
    bool text_from_label;
    CuContext *in_ctx, *out_ctx;
    CuControlsUtils u;
};

/** \brief Constructor with the parent widget, an *engine specific* Cumbia implementation and a CuControlsReaderFactoryI interface.
 *
 *  Please refer to \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 */
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

/** \brief Constructor with the parent widget, *CumbiaPool*  and *CuControlsFactoryPool*
 *
 *   Please refer to \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 */
QuCheckBox::QuCheckBox(QWidget *parent, CumbiaPool *cu_poo, const CuControlsFactoryPool &f_poo)
    : QCheckBox(parent)
{
    d = new QuCheckboxPrivate;
    m_init();
    d->out_ctx = new CuContext(cu_poo, f_poo);
    d->in_ctx = new CuContext(cu_poo, f_poo);
}

/** \brief the class destructor
 *
 * Deletes the input and output contexts
 */
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
    d->ok = false;
    d->text_from_label = true;
    setText("No Link");
    connect(this, SIGNAL(clicked(bool)), this, SLOT(checkboxClicked(bool)));
}

QString QuCheckBox::source() const
{
    if(CuControlsReaderA* r = d->out_ctx->getReader())
        return r->source();
    return "";
}

/*! \brief provided for convenience/compatibility, returns source
 *
 * Source and target are the same for QuCheckBox
 *
 * @return the value returned by source
 *
 * @see source
 * @see setSource
 * @see setTarget
 */
QString QuCheckBox::target() const
{
    return source();
}

/** \brief Connect the reader and the writer to the specified source name.
 *
 * If a reader with a different source is configured, it is deleted.
 * The same goes for the writer.
 * If options have been set with CuContext::setOptions, they are used to set up the reader as desired.
 *
 * \note the reader and the writer are connected to the same source.
 *
 * Refer to \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 *
 * @see CuContext::setOptions
 * @see source
 */
void QuCheckBox::setSource(const QString &s)
{
    CuControlsReaderA * r = d->out_ctx->replace_reader(s.toStdString(), this);
    if(r)
        r->setSource(s);
    CuControlsWriterA *w = d->in_ctx->replace_writer(s.toStdString(), this);
    if(w)
        w->setTarget(s);
}

/*! \brief provided for convenience/compatibility, calls setSource
 *
 * Calls setSource.
 * Source and target are the same for QuCheckBox
 *
 * @see setSource
 * @see source
 * @see target
 */
void QuCheckBox::setTarget(const QString &t)
{
    setSource(t);
}

/** \brief set the textFromLabel property to true
 *
 * @param tfl true the text of the check box is taken from the configuration data,
 *        taking the "label" value stored on the CuData (this is done within onUpdate)
 * @param tfl false the "label" value possibly stored in the configuration data is
 *        ignored.
 */
void QuCheckBox::setTextFromLabel(bool tfl)
{
    d->text_from_label = tfl;
    if(tfl && property("label").isValid())
        setText(property("label").toString());
}

/** \brief Reimplemented from QWidget's contextMenuEvent, emits a signal
 *         to request statistics on the link.
 *
 *  A pop up menu appears and an action to view link statistics is made available.
 */
void QuCheckBox::contextMenuEvent(QContextMenuEvent *e)
{
    CuContextMenu* m = findChild<CuContextMenu *>();
    if(!m)
        m = new CuContextMenu(this);
    m->popup(e->globalPos(), this);
}

void QuCheckBox::checkboxClicked(bool checked)
{
      CuVariant arg(checked);
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

/** \brief Returns the reader's context
 *
 * @return a pointer to the output CuContext used to read.
 */
CuContext *QuCheckBox::getOutputContext() const
{
    return d->out_ctx;
}

/** \brief Returns the writer's context
 *
 * @return a pointer to the input CuContext used to write.
 */
CuContext *QuCheckBox::getInputContext() const
{
    return d->in_ctx;
}

/** \brief returns the textFromLabel property value
 *
 * @return true: enabled
 * @return false: disabled
 *
 * @see setTextFromLabel
 */
bool QuCheckBox::textFromLabel() const
{
    return d->text_from_label;
}

/** \brief determines the behaviour of the checkbox as a reader
 *
 * Implements CuDataListener::onUpdate
 *
 * \li if data is of type "property", the checkbox text is configured according to the
 *     "label" value, if available
 *
 * \li if an error occurred, the widget is disabled
 * \li a message is set as a tooltip according to the "msg" value of da.
 * \li if the "value" contained in da can be converted to a boolean, the
 *     checkbox is set accordingly. If not, it is set as *partially checked*.
 *
 */
void QuCheckBox::onUpdate(const CuData &da)
{
    d->ok = !da["err"].toBool();
    bool write_op = da["activity"].toString() == "writer";
    const QString& msg = d->u.msg(da);
    if(d->ok && d->auto_configure && da["type"].toString() == "property" &&
            da.containsKey("label")) {
        setProperty("label", QString::fromStdString(da["label"].toString()) );
        if(d->text_from_label)
            setText(property("label").toString());
    }
    if(!d->ok && write_op) {
        Cumbia* cumbia = d->in_ctx->cumbia();
        if(!cumbia) /* pick from the CumbiaPool */
            cumbia = d->in_ctx->cumbiaPool()->getBySrc(da["src"].toString());
        CuLog *log;
        if(cumbia && (log = static_cast<CuLog *>(cumbia->getServiceProvider()->get(CuServices::Log))))
        {
            static_cast<QuLogImpl *>(log->getImpl("QuLogImpl"))->showPopupOnMessage(CuLog::CategoryWrite, true);
            log->write(QString("QuApplyNumeric [" + objectName() + "]").toStdString(), msg.toStdString(), CuLog::LevelError, CuLog::CategoryWrite);
        }
    }
    setEnabled(d->ok);
    // update link statistics
    d->out_ctx->getLinkStats()->addOperation();
    // tooltip message
    setToolTip(msg);

    if(!d->ok && !write_op)
        d->out_ctx->getLinkStats()->addError(msg.toStdString());
    else if(da.containsKey("value"))
    {
        CuVariant val = da["value"];
        if(val.getType() == CuVariant::Boolean) {
            d->last_val = val.toBool();
            d->last_val ? setCheckState(Qt::Checked) : setCheckState(Qt::Unchecked);
        }
        else {
            setChecked(Qt::PartiallyChecked);
            setToolTip(QString("wrong data type [%1]").arg(QString::fromStdString(val.dataTypeStr(val.getType()))));
        }
    }
}

