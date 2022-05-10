#include "qulabel.h"
#include "cucontrolsreader_abs.h"
#include <cumacros.h>
#include <cumbiapool.h>
#include <cudata.h>
#include <qustringlist.h>
#include <qustring.h>
#include <QContextMenuEvent>
#include <QPainter>
#include <QPaintEvent>
#include <cucontrolsutils.h>

#include "qupalette.h"
#include "cucontrolsfactories_i.h"
#include "cucontrolsfactorypool.h"
#include "culinkstats.h"
#include "cucontextmenu.h"
#include "cucontext.h"

/** @private */
class QuLabelPrivate
{
public:
    bool auto_configure;
    bool read_ok;
    bool display_u_enabled;
    QString display_u;
    QuPalette palette;
    int max_len;
    CuContext *context;
    CuControlsUtils u;
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
    d->display_u = QString::fromStdString(da["display_unit"].toString());
    QString fmt = QString::fromStdString(da["format"].toString());
    if(format().isEmpty() && !fmt.isEmpty())
        setFormat(fmt);
    // get colors and strings, if available
    QColor c;
    QString s;
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

QString QuLabel::source() const
{
    if(CuControlsReaderA* r = d->context->getReader())
        return r->source();
    return "";
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

void QuLabel::contextMenuEvent(QContextMenuEvent *e)
{
    CuContextMenu* m = findChild<CuContextMenu *>();
    if(!m)
        m = new CuContextMenu(this);
    m->popup(e->globalPos(), d->context);
}

void QuLabel::onUpdate(const CuData &da)
{
    bool background_modified = false;
    QString txt;
    QColor background, border;
    d->read_ok = !da["err"].toBool();

    // update link statistics
    d->context->getLinkStats()->addOperation();
    if(!d->read_ok)
        d->context->getLinkStats()->addError(da["msg"].toString());

    d->read_ok ? border = d->palette["dark_green"] : border = d->palette["dark_red"];

    setToolTip(d->u.msg(da));
    setBorderColor(border);

    if(!d->read_ok)
        setText("####");
    else {
        if(d->read_ok && d->auto_configure && da["type"].toString() == "property") {
            m_configure(da);
            emit propertyReady(da);
        }
        if(da.containsKey("value"))
        {
            CuVariant val = da["value"];
            QuLabelBase::setValue(val, &background_modified);
            if(d->display_u_enabled && !d->display_u.isEmpty())
                setText(text() + " [" + d->display_u + "]");
        }
    }

    if(da.containsKey("sc")) {
        CuVariant v = da["sc"];
        background = d->palette[QString::fromStdString(v.toString())];
        if(background.isValid())
            setBackground(background);
    }
    else if(!background_modified) {
        // background has not already been set by QuLabelBase::setValue (this happens if either a
        // boolean display or enum display have been configured)
        // if so, use the "qc" as a background
        if(da.containsKey("qc"))
            background = d->palette[QString::fromStdString(da["qc"].toString())];
        setBackground(background); // checks if background is valid
    }

    emit newData(da);
}
