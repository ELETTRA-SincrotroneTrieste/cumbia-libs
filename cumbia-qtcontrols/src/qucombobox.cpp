#include "qucombobox.h"
#include "cucontrolswriter_abs.h"
#include "cuengine_swap.h"
#include <cumacros.h>
#include <cumbiapool.h>
#include <cudata.h>
#include <QContextMenuEvent>
#include <QMetaProperty>
#include <QStringList>
#include <vector>

#include <cucontrolsfactories_i.h>
#include <cucontrolsfactorypool.h>
#include <culinkstats.h>
#include <cucontextmenu.h>
#include <cucontext.h>
#include <culog.h>
#include <cuserviceprovider.h>
#include <cuservices.h>
#include <qulogimpl.h>
#include <quanimation.h>
#include <qustringlist.h>
#include <quvector.h>
#include <qustring.h>
#include <cucontrolsutils.h>

/** @private */
class QuComboBoxPrivate
{
public:
    bool auto_configure;
    bool ok;
    bool index_mode, execute_on_index_changed;
    CuContext *context;
    QuAnimation animation;
    // for private use
    bool configured, index_changed_connected;
    CuControlsUtils u;
};

/** \brief Constructor with the parent widget, an *engine specific* Cumbia implementation and a CuControlsReaderFactoryI interface.
 *
 *  Please refer to \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 */
QuComboBox::QuComboBox(QWidget *w, Cumbia *cumbia, const CuControlsWriterFactoryI &r_factory) :
    QComboBox(w), CuDataListener()
{
    m_init();
    d->context = new CuContext(cumbia, r_factory);
    std::vector<std::string> vs = { "values" };
    d->context->setOptions(CuData("fetch_props", vs ));
}

/** \brief Constructor with the parent widget, *CumbiaPool*  and *CuControlsFactoryPool*
 *
 *   Please refer to \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 */
QuComboBox::QuComboBox(QWidget *w, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool) :
    QComboBox(w), CuDataListener()
{
    m_init();
    d->context = new CuContext(cumbia_pool, fpool);
    std::vector<std::string> vs = { "values" };
    d->context->setOptions(CuData("fetch_props", vs ));
}

void QuComboBox::m_init()
{
    d = new QuComboBoxPrivate;
    d->context = NULL;
    d->auto_configure = true;
    d->ok = false;
    d->index_mode = false;
    d->execute_on_index_changed = false;
    d->configured = d->index_changed_connected = false; // for private use
    d->animation.setType(QuAnimation::RayOfLight);
}

QuComboBox::~QuComboBox()
{
    pdelete("~QuComboBox %p", this);
    delete d->context;
    delete d;
}

/** \brief returns the target of the writer
 *
 * @return a QString with the name of the target
 */
QString QuComboBox::target() const {
    CuControlsWriterA *w = d->context->getWriter();
    if(w != NULL)
        return w->target();
    return "";
}

/** \brief returns the pointer to the CuContext
 *
 * CuContext sets up the connection and is used as a mediator to send and get data
 * to and from the reader.
 *
 * @see CuContext
 */
CuContext *QuComboBox::getContext() const
{
    return d->context;
}

/*! \brief returns index mode enabled or not
 *
 * @return true: the combo box is working in index mode
 * @return false: the combo box is working with its text items
 *
 * When index mode is enabled, the current item is chosen by its index
 * at configuration time and the value used for writing is the QComboBox currentIndex.
 * When it is disabled, currentText is used for writings and the current item
 * at configuration time is searched by the source value (string comparison).
 */
bool QuComboBox::indexMode() const
{
    return  d->index_mode;
}

/*! \brief returns the value of the property executeOnIndexChanged
 *
 * @return true when the index of the combo box changes, a write operation is sent through the link
 * @return false no writings are executed upon index change
 *
 * @see setExecuteOnIndexChanged
 */
bool QuComboBox::executeOnIndexChanged() const
{
    return  d->execute_on_index_changed;
}

QString QuComboBox::getData() const
{
    if(d->index_mode)
        return QString::number(currentIndex());
    return currentText();
}

/** \brief Connect the reader to the specified source.
 *
 * If a reader with a different source is configured, it is deleted.
 * If options have been set with QuContext::setOptions, they are used to set up the reader as desired.
 *
 * @see QuContext::setOptions
 * @see source
 */
void QuComboBox::setTarget(const QString &target, CuContext *ctx) {
    if(ctx) {
        delete d->context;
        d->context = ctx;
    }
    CuControlsWriterA* w = d->context->replace_writer(target.toStdString(), this);
    if(w)
        w->setTarget(target);
}

void QuComboBox::clearTarget() {
    d->context->disposeWriter();
}

bool QuComboBox::ctxSwap(CumbiaPool *cu_p, const CuControlsFactoryPool &fpool) {
    CuEngineSwap csw;
    d->context = csw.replace(this, d->context, cu_p, fpool);
    return csw.ok();
}

void QuComboBox::contextMenuEvent(QContextMenuEvent *e)
{
    CuContextMenu* m = findChild<CuContextMenu *>();
    if(!m) m = new CuContextMenu(this);
    m->popup(e->globalPos(), this);
}

void QuComboBox::paintEvent(QPaintEvent *pe)
{
    QComboBox::paintEvent(pe);
    if(d->animation.currentValue() != 0.0) {
        d->animation.draw(this, pe->rect());
    }
}

void QuComboBox::m_configure(const CuData& da)
{
    d->ok = !da[TTT::Err].toBool();  // da["err"]

    CuControlsUtils u;
    setDisabled(!d->ok);
    QString toolTip(u.msg(da));

    if(d->ok) {
        QString description, unit, label;
        CuVariant items;

        unit = QuString(da, "display_unit");
        label = QuString(da, "label");
        description = QuString(da, "description");

        setProperty("description", description);
        setProperty("unit", unit);

        // QuStringList will be nonempty only if the "values" key is found (and nonempty)
        foreach(QString s, QuStringList(da, "values"))
           insertItem(count(), s);


        // initialise the object with the "write" value (also called "set point"), if available:
        //
        if(da.containsKey(TTT::WriteValue)) {  // da.containsKey("w_value")
            bool ok;
            int index = -1;
            if(d->index_mode) {
                ok = da[TTT::WriteValue].to<int>(index);  // da["w_value"]
                if(ok && index > -1) { // conversion successful
                    setCurrentIndex(index);
                }
            }
            else {
                std::string as_s = da[TTT::WriteValue].toString(&ok);  // da["w_value"]
                if(ok) {
                    index = findText(QuString(as_s));
                    if(index > -1)
                        setCurrentIndex(index);
                }
            }
        }

        toolTip.append("\n");
        if(!label.isEmpty()) toolTip += label + " ";
        if(!unit.isEmpty()) toolTip += "[" + unit + "] ";
        if(!description.isEmpty()) toolTip += "\n" + description;

        setToolTip(toolTip);

        if(d->execute_on_index_changed && !d->index_changed_connected) {
            d->index_changed_connected = true;
            connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(m_onIndexChanged(int)));
        }
        d->configured = true;
    }
    else { // !ok
        setToolTip(toolTip);
    }
}


void QuComboBox::onUpdate(const CuData &da)
{
    d->ok = !da[TTT::Err].toBool();  // da["err"]
    QString mes(d->u.msg(da));
    // update link statistics
    d->context->getLinkStats()->addOperation();
    if(!d->ok)
        d->context->getLinkStats()->addError(mes.toStdString());

    if(!d->ok) {
        perr("QuComboBox [%s]: error %s target: \"%s\" format %s (writable: %d)", qstoc(objectName()),
             da[TTT::Src].toString().c_str(), mes.toStdString().c_str(),  // da["src"]
                da[TTT::DataFormatStr].toString().c_str(), da["writable"].toInt());  // da["dfs"]

        Cumbia* cumbia = d->context->cumbia();
        if(!cumbia) /* pick from the CumbiaPool */
            cumbia = d->context->cumbiaPool()->getBySrc(da[TTT::Src].toString());  // da["src"]
        CuLog *log;
        if(cumbia && (log = static_cast<CuLog *>(cumbia->getServiceProvider()->get(CuServices::Log))))
        {
            static_cast<QuLogImpl *>(log->getImpl("QuLogImpl"))->showPopupOnMessage(CuLog::CategoryWrite, true);
            log->write(QString("QuApplyNumeric [" + objectName() + "]").toStdString(), mes.toStdString(), CuLog::LevelError, CuLog::CategoryWrite);
        }
    }
    else if(d->auto_configure &&  da.has(TTT::Type, "property")) {  // has("type", "property")
        //
        // --------------------------------------------------------------------------------------------
        // You may want to check data format and write type and issue a warning or avoid configuration
        // at all if they are not as expected
        // if(da["dfs"] == "scalar" && da["writable"].toInt() > 0)
        // --------------------------------------------------------------------------------------------
        m_configure(da);
        d->animation.installOn(this);
    }
    d->ok ? d->animation.setPenColor(QColor(Qt::green)) : d->animation.setPenColor(QColor(Qt::red));
    d->ok ? d->animation.setDuration(1500) : d->animation.setDuration(3000);
    d->animation.start();
    emit newData(da);
}

void QuComboBox::m_onIndexChanged(int i) {
    d->animation.setPenColor(QColor(Qt::gray));
    d->animation.loop(QuAnimation::RayOfLight);
    if(d->index_mode) {
        write(i);
    }
    else {
        write(currentText());
    }
}

/** \brief write an integer to the target
 *
 * @param i the value to be written on the target
 */
void QuComboBox::write(int ival) {
    m_write(CuVariant(ival));
}

/** \brief write a string to the target
 *
 * @param s the string to be written on the target
 */
void QuComboBox::write(const QString& s) {
    m_write(CuVariant(s.toStdString()));
}

/*! \brief enable or disable index mode
 *
 * @param im true index mode enabled
 * @param im false index mode disabled
 * @see indexMode
 */
void QuComboBox::setIndexMode(bool im) {
    d->index_mode = im;
}

/*! \brief change the executeOnIndexChanged property
 *
 * @param exe true: write on index change according to indexMode
 * @param exe false (default) noop on index change
 *
 * @see executeOnIndexChanged
 */
void QuComboBox::setExecuteOnIndexChanged(bool exe)
{
    d->execute_on_index_changed = exe;
    if(!exe) {
        disconnect(this, SIGNAL(currentIndexChanged(int)));
        d->index_changed_connected = false;
    }
    if(d->configured && !d->index_changed_connected && exe) {
        d->index_changed_connected = true;
        connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(m_onIndexChanged(int)));
    }
}

void QuComboBox::onAnimationValueChanged(const QVariant &)
{
    // optimize animation paint area with the clipped rect provided by QuAnimation::clippedRect
    update(d->animation.clippedRect(rect()));
}

// perform the write operation on the target
//
void QuComboBox::m_write(const CuVariant& v){
    CuControlsWriterA *w = d->context->getWriter();
    if(w) {
        w->setArgs(v);
        w->execute();
    }
}
