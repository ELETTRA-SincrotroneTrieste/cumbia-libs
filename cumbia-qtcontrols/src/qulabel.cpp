#include "qulabel.h"
#include "cucontrolsreader_abs.h"
#include <cumacros.h>
#include <cumbiapool.h>
#include <cudata.h>
#include <qustringlist.h>
#include <qustring.h>
#include <QStringBuilder>
#include <QContextMenuEvent>
#include <QPainter>
#include <QPaintEvent>
#include <cucontrolsutils.h>
#include <QToolTip>
#include <QHelpEvent>
#include <cuengineaccessor.h>
#include <quapplication.h>

#include "qupalette.h"
#include "cucontrolsfactories_i.h"
#include "cucontrolsfactorypool.h"
#include "culinkstats.h"
#include "cucontextmenu.h"
#include "cucontext.h"
#include "cuengine_swap.h"
#include "cumouse-ev-handler.h"

/** @private */
class QuLabelPrivate
{
public:
    bool auto_configure;
    bool read_ok;
    bool display_u_enabled;
    QuPalette palette;
    int max_len;
    CuContext *context;
    CuControlsUtils u;
    CuMouseEvHandler mouseh;
    char display_u[16];
    char msg[MSGLEN];

    CuData last_d;
};

/** \brief Constructor with the parent widget, an *engine specific* Cumbia implementation and a CuControlsReaderFactoryI interface.
 *
 *  Please refer to \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 */
QuLabel::QuLabel(QWidget *w, Cumbia *cumbia, const CuControlsReaderFactoryI &r_factory) :
    QuLabelBase(w), CuDataListener()
{
    m_init();
    d->context = new CuContext(cumbia, r_factory);
    m_initCtx();
}

/** \brief Constructor with the parent widget, *CumbiaPool*  and *CuControlsFactoryPool*
 *
 *   Please refer to \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 */
QuLabel::QuLabel(QWidget *w, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool) :
    QuLabelBase(w), CuDataListener()
{
    m_init();
    d->context = new CuContext(cumbia_pool, fpool);
    m_initCtx();
}

/*!
 * \brief Classical, single parent-widget constructor. *QuApplication* properly initialized with
 *        cumbia engine objects is compulsory.
 *
 * \param parent widget
 * \par Important note: cumbia engine references are obtained from the QuApplication instance.
 *      For best performance, static cast of QCoreApplication::instance() to QuApplication is
 *      used.
 * \since cumbia 2.1
 */
QuLabel::QuLabel(QWidget *w) : QuLabelBase(w), CuDataListener() {
    QuApplication *a = static_cast<QuApplication *>(QCoreApplication::instance());
    m_init();
    d->context = new CuContext(a->cumbiaPool(), *a->fpool());
    m_initCtx();
}

void QuLabel::m_init()
{
    d = new QuLabelPrivate;
    d->context = NULL;
    d->auto_configure = true;
    d->read_ok = false;
    d->display_u_enabled = true;
    d->max_len = -1;
    setProperty("trueString", "TRUE");
    setProperty("falseString", "FALSE");
    setProperty("trueColor", QColor(Qt::green));
    setProperty("falseColor", QColor(Qt::red));
    QColor background = d->palette["white"];
    QColor border = d->palette["gray"];
    setDecoration(background, border);
    d->msg[0] = '\0';
    d->display_u[0] = '\0';
}

void QuLabel::m_initCtx()
{
    std::vector<std::string> props;
    props.push_back("colors");
    props.push_back("values");
    d->context->setOptions(CuData("fetch_props", props));
}

void QuLabel::m_configure(const CuData &da)
{
    const char* u = da["display_unit"].c_str(), *fm = da[TTT::NumberFormat].c_str();
    u != nullptr && strlen(u) > 0 ? snprintf(d->display_u, 16, " [%s]", u) : d->display_u[0] = '\0';
    if(format().isEmpty() && fm != nullptr)
        setFormat(fm);
    // get colors and strings, if available
    QColor c;
    // colors and labels will be empty if "colors" and "labels" are not found
    QuStringList colors(da, "colors"), labels(da, "values");

    for(int i = 0; i < qMax(colors.size(), labels.size()); i++) {
        setEnumDisplay(i, i < labels.size() ? labels[i] : "-",
                       i < colors.size() ? c = d->palette[colors[i]] : c = QColor(Qt::white));
    }
}

QuLabel::~QuLabel()
{
    pdelete("~QuLabel %p", this);
    delete d->context;
    delete d;
}

QString QuLabel::source() const {
    return d->context && d->context->getReader() ?
               d->context->getReader()->source() : QString();
}

/** \brief returns the pointer to the CuContext
 *
 * CuContext sets up the connection and is used as a mediator to send and get data
 * to and from the reader.
 *
 * @see CuContext
 */
CuContext *QuLabel::getContext() const
{
    return d->context;
}

/*! \brief returns true if the display unit is shown beside the value, false otherwise
 *
 * @return true the display unit is shown
 * @return false only the value is shown
 *
 * \par note The source must provide a non empty "display_unit" property
 *
 * @see setDisplayUnitEnabled
 */
bool QuLabel::displayUnitEnabled() const
{
    return d->display_u_enabled;
}

/*! \brief returns the display unit read by the configuration properties at
 *         initialization time.
 *
 * @returns a string containing the display unit read from the initial configuration, if available
 *          from the source.
 *
 * @see displayUnitEnabled
 *
 */
QString QuLabel::displayUnit() const
{
    return d->display_u;
}

/** \brief set a custom QuPalette to change the string/color association
 *
 * @param colors a new QuPalette that replaces the current one
 *
 * QuPalette maps color names to QColor colors.
 * You can use this method as a trick to change the color for a state represented by the label.
 * For example, if normally an *OK* state is associated to "green", you can set a custom palette
 * so that the "green" color string is mapped into white color.
 * In the following example, white color is used with the *green* key:
 *
 * \code
 * QuPalette myPalette = stateWidget->quPalette();
 * myPalette["green"] = QColor(Qt::white);
 * stateWidget->setQuPalette(myPalette);
 * \endcode
 *
 * Altering the QuPalette of a QuLabel (or a QuLed) is useful when a given state is associated
 * to a predefined color (e.g. ON:green, OFF:white, ALARM:yellow, FAULT:red) and you want different
 * combinations. Cumbia engines must store a *state_color* key/value pair in the *CuVariant* data
 * delivered by the *onUpdate* method when the engine itself handles a *"s"* data type.
 *
 * @see quPalette
 */
void QuLabel::setQuPalette(const QuPalette &colors) {
    d->palette = colors;
}

/** \brief returns the QuPalette currently used
 *
 * @return QuPalette in use
 * @see setQuPalette
 */
QuPalette QuLabel::quPalette() const {
    return d->palette;
}

/** \brief Connect the reader to the specified source.
 *
 * If a reader with a different source is configured, it is deleted.
 * If options have been set with QuContext::setOptions, they are used to set up the reader as desired.
 *
 * @see QuContext::setOptions
 * @see source
 */
void QuLabel::setSource(const QString &s) {
    CuControlsReaderA * r = d->context->replace_reader(s.toStdString(), this);
    if(r)
        r->setSource(s);
}

void QuLabel::unsetSource()
{
    d->context->disposeReader();
}

/*! \brief enable or disable the display unit visualization.
 *
 * See displayUnitEnabled documantation for further details.
 */
void QuLabel::setDisplayUnitEnabled(bool en)
{
    d->display_u_enabled = en;
}

bool QuLabel::ctxSwap(CumbiaPool *c_p, const CuControlsFactoryPool &fpool) {
    CuEngineSwap csw;
    d->context = csw.replace(this, d->context, c_p, fpool);
    return csw.ok();
}

void QuLabel::onUpdate(const CuData &da) {
    //    auto start = std::chrono::high_resolution_clock::now();
    const bool& ok = !da[TTT::Err].toBool();
    const char *mode = da[TTT::Mode].c_str();
    bool event = mode != nullptr && strcmp(mode, "E") == 0;
    bool update = event; // if data is delivered by an event, always refresh
    // update link statistics
    d->context->getLinkStats()->addOperation();
    if(!event) { // data generated periodically by a poller or by something else
        // update label if value changed
        update = !ok || d->last_d[TTT::Value] != da[TTT::Value];
        if(strlen(d->msg) > 0)
            d->msg[0] = 0; // clear msg
    }
    else { // "event" mode
        d->u.msg_short(da, d->msg);
    }
    if(update) {
        QColor bg; // background color
        if(!ok)
            d->context->getLinkStats()->addError(da.c_str(TTT::Message));

        if(ok != d->read_ok) {
            QColor border;
            ok ? border = d->palette["dark_green"] : border = d->palette["dark_red"];
            setBorderColor(border);
            d->read_ok = ok;
        }

        if(!ok) {
            setText("####");
        }
        else {
            if(d->read_ok && d->auto_configure && da[TTT::Type].toString() == "property") {
                m_configure(da);
                emit propertyReady(da);
            }
            if(da.containsKey(TTT::Value))  {
                const CuVariant& val = da[TTT::Value];
                // bg will be a valid color if color 'maps' (e.g. true/false colors) are
                // defined as properties. In no other case decode sets a color
                QuLabelBase::decode(val, bg);
                if(d->display_u_enabled && strlen(d->display_u) > 0 &&
                    strlen(d_data->text) + strlen(d->display_u) + 1 < QULABEL_MAXLEN) {
                    strncat(d_data->text, d->display_u, 16);
                }
                setText(d_data->text);
            }
        }
        if(bg.isValid()) { // from QuLabelBase.decode
            // bg valid: colors were associated to values by properties. This has highest pri
            setBackground(bg);
        }
        else {
            // check for state color string change before accessing palette
            const char* statc = da.c_str(TTT::StateColor);
            const std::string& oldc = d->last_d.s(TTT::StateColor);
            if((statc && oldc.length() == 0) || (statc && strcmp(statc, oldc.c_str()) != 0)) {
                bg = d->palette[statc];
                if(bg.isValid())
                    setBackground(bg);
            }
            else if(!bg.isValid() && !statc) {
                const char *qc = da.c_str(TTT::QualityColor);
                const char *lastqc = d->last_d.c_str(TTT::QualityColor);
                // background has not already been set by QuLabelBase::setValue (this happens if either a
                // boolean display or enum display have been configured)
                // if so, use the "qc" as a background
                if((qc && !lastqc) || ( qc && lastqc && strcmp(qc,lastqc) != 0)) { // quality color changed
                    const QColor &bg = d->palette[qc];
                    if(bg.isValid())
                        setBackground(bg); // checks if background is valid
                }
            }
        }
        // onUpdate measures better with d->last_d = da; than with d->last_d = da.clone()
        // even though measured alone the clone version performs better
        //        d->last_d = da.clone(); // clone does a copy, then contents moved into last_d
        d->last_d = da;
    } // end if(update)
    //    auto end = std::chrono::high_resolution_clock::now();
    //    auto  duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    //    std::cout << "Elapsed time QuLabel.update: " << duration.count() << " microseconds" << std::endl;

    emit newData(da);

}

void QuLabel::contextMenuEvent(QContextMenuEvent *e) {
    CuContextMenu* m = new CuContextMenu(this);
    m->popup(e->globalPos(), this); // menu auto deletes after exec
}

void QuLabel::mousePressEvent(QMouseEvent *e) {
    d->mouseh.pressed(e, this, this);
}

void QuLabel::mouseReleaseEvent(QMouseEvent *e){
    d->mouseh.released(e, this, this);
}

void QuLabel::mouseMoveEvent(QMouseEvent *e) {
    d->mouseh.moved(e, this, this);
}

void QuLabel::mouseDoubleClickEvent(QMouseEvent *e) {
    d->mouseh.doubleClicked(e, this, this);
}

bool QuLabel::event(QEvent *e) {
    if(e->type() == QEvent::ToolTip) {
        if(strlen(d->msg) == 0) {
            d->u.msg_short(d->last_d, d->msg);
        }
        QToolTip::showText(static_cast<QHelpEvent *>(e)->globalPos(), d->msg);
        return true;
    }
    return QLabel::event(e);
}
