#include "botreader.h"
#include "cucontrolsreader_abs.h"
#include <cumacros.h>
#include <cumbiapool.h>
#include <cudata.h>
#include <cudataquality.h>
#include <cuformulaparsehelper.h>
#include <tsource.h>
#include <cuformulaparsehelper.h>
#include <QContextMenuEvent>
#include <QMetaProperty>
#include <QScriptEngine>
#include <QRegularExpression>
#include <QtDebug>


#include "cucontrolsfactories_i.h"
#include "cucontrolsfactorypool.h"
#include "culinkstats.h"
#include "cucontextmenu.h"
#include "cucontext.h"
#include "formulahelper.h"
#include "botreadquality.h"

/** @private */
class BotReaderPrivate
{
public:
    bool auto_configure, props_only;
    bool read_ok;
    bool monitor;
    QString host, source;
    QString command;
    BotReader::RefreshMode refresh_mode;
    QString desc, label, unit, print_format;
    double max,min;
    CuContext *context;
    int user_id, chat_id;
    int ttl, poll_period;
    int refresh_cnt, notify_cnt;
    BotReader::Priority priority;
    QDateTime startDateTime;
    int index; // to find the reader by index (shortcuts)
    CuDataQuality old_quality; // constructor sets internal quality to Valid
    CuVariant old_value;
    bool formula_is_identity;
};

/** \brief Constructor with the parent widget, *CumbiaPool*  and *CuControlsFactoryPool*
 *
 *   Please refer to \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 */
BotReader::BotReader(int user_id,
                     int chat_id,
                     QObject *w,
                     CumbiaPool *cumbia_pool,
                     const CuControlsFactoryPool &fpool,
                     int ttl, int poll_period,
                     const QString &cmd,
                     Priority pri,
                     const QString &host,
                     bool monitor) :
    QObject(w), CuDataListener()
{
    m_init();
    d->context = new CuContext(cumbia_pool, fpool);
    d->chat_id = chat_id;
    d->user_id = user_id;
    d->ttl = ttl;
    d->priority = pri;
    d->monitor = monitor;
    d->command = cmd;
    d->index = -1;
    d->poll_period = poll_period;
    d->refresh_mode = RefreshModeUndefined;
    const char* env_tg_host = nullptr;
    if(host.isEmpty() && (env_tg_host = secure_getenv("TANGO_HOST")) != nullptr) {
        d->host = QString(env_tg_host);
    }
    else
        d->host = host;
}

void BotReader::m_init()
{
    d = new BotReaderPrivate;
    d->context = NULL;
    d->auto_configure = true;
    d->props_only = false;
    d->read_ok = false;
    d->refresh_cnt = d->notify_cnt = 0;
    d->max = d->min = 0.0;
    d->old_quality = CuDataQuality(CuDataQuality::Undefined);
    d->formula_is_identity = false;
}

BotReader::~BotReader()
{
    printf("\e[1;31m~BotReader %p\e[0m\n", this);
    delete d->context;
    delete d;
}

void BotReader::setPropertyEnabled(bool get_props)
{
    d->auto_configure = get_props;
}

void BotReader::setPropertiesOnly(bool props_only)
{
    d->props_only = props_only;
}

/**
 * @brief BotReader::sources returns the list of sources identified in d->source
 *
 * @return a list of sources identified in d->source. If d->source is a formula,
 *         more than one source may be involved. Otherwise, the method returns the
 *         only source set with setSource
 */
QStringList BotReader::sources() const
{
    CuFormulaParseHelper ph;
    return ph.sources(d->source);
}

QString BotReader::source() const
{
    return d->source; // without host
}

/**
 * @brief BotReader::hasSource returns true if the reader sources contain src, false otherwise
 * @param src the source to search for
 * @return true if src in sources, false otherwise
 *
 * @see sources
 */
bool BotReader::hasSource(const QString &src) const
{
    return sources().contains(src);
}

/**
 * @brief BotReader::sourceMatch finds if one source matches the given patter (regexp match)
 * @param pattern the regexp pattern
 * @return true if *one of* the sources matches pattern
 *
 * \par Example
 * \code
 * reader->setSource("{test/device/1/long_scalar, test/device/2/double_scalar}(@1 + @2)");
 * bool match = sourceMatch("double_scalar");
 * // match is true
 *
 * \endcode
 */
bool BotReader::sourceMatch(const QString &pattern) const
{
    return CuFormulaParseHelper().sourceMatch(d->source, pattern);
}

bool BotReader::sameSourcesAs(const QSet<QString> &srcset) const
{
    return sources().toSet() == srcset;
}

/**
 * @brief BotReader::formulaIsIdentity returns true if the source of the reader does not
 *        contain a formula, such as "test/device/1/double_scalar", false otherwise
 *
 * @return true: this is a plain reader of a single source, no further calculations are made
 * @return false: this is a reader with one or more sources and the result implies a calculation,
 *         for example  test/device/1/double_scalar - 2 * test/device/2/double_scalar
 */
bool BotReader::hasNoFormula() const
{
    return d->formula_is_identity;
}

QString BotReader::command() const
{
    return d->command;
}

void BotReader::setCommand(const QString &cmd)
{
    d->command = cmd;
}

QString BotReader::host() const
{
    return d->host;
}

QDateTime BotReader::startedOn() const
{
    return d->startDateTime;
}

int BotReader::ttl() const
{
    return d->ttl;
}

void BotReader::setStartedOn(const QDateTime &dt)
{
    if(d->startDateTime.isValid()) {
        perr("BotReader.setStartedOn: cannot set start date time more than once");
    }
    else {
        d->startDateTime = dt;
    }
}

bool BotReader::hasStartDateTime() const
{
    return d->startDateTime.isValid();
}

/** \brief returns the pointer to the CuContext
 *
 * CuContext sets up the connection and is used as a mediator to send and get data
 * to and from the reader.
 *
 * @see CuContext
 */
CuContext *BotReader::getContext() const
{
    return d->context;
}

int BotReader::userId() const
{
    return d->user_id;
}

int BotReader::chatId() const
{
    return d->chat_id;
}

BotReader::Priority BotReader::priority() const
{
    return d->priority;
}

/**
 * @brief BotMonitor::index useful to find a Reader by its unique index
 *
 * The purpose of an index is to create a "/shortcut" to easily find a
 * reader by its unique id.
 *
 * The id is unique for every reader *per user_id*
 *
 * @return the BotReader index
 */
int BotReader::index() const
{
    return d->index;
}

void BotReader::setIndex(int idx)
{
    d->index = idx;
}

int BotReader::refreshCount() const
{
    return  static_cast<int>(d->refresh_cnt);
}

int BotReader::notifyCount() const
{
    return d->notify_cnt;
}

BotReader::RefreshMode BotReader::refreshMode() const
{
    return d->refresh_mode;
}

void BotReader::setPeriod(int period) const
{
    CuData p("period", period);
    p["src"] = d->source.toStdString();
    d->context->sendData(p);
}

int BotReader::period() const
{
    int p;
    CuData o;
    d->context->getData(o);
    o.containsKey("period") ? p = o["period"].toInt() : p = -1;
    return p;
}

QString BotReader::print_format() const
{
    return d->print_format;
}

double BotReader::min() const
{
    return  d->min;
}

QString BotReader::description() const
{
    return d->desc;
}

QString BotReader::label() const
{
    return d->label;
}

void BotReader::setPriority(BotReader::Priority pri)
{
    if(pri != d->priority) {
        Priority oldpri = d->priority;
        d->priority = pri;
        emit priorityChanged(d->chat_id, source(), oldpri, d->priority);
    }
}

/** \brief Connect the reader to the specified source.
 *
 * If a reader with a different source is configured, it is deleted.
 * If options have been set with QuContext::setOptions, they are used to set up the reader as desired.
 *
 * @see QuContext::setOptions
 * @see source
 */
void BotReader::setSource(const QString &s)
{
    CuData options;
    if(!d->auto_configure)
        options["no-properties"] = true;
    if(d->props_only)
        options["properties-only"] = true;
    if(d->monitor)
        options["period"] = d->poll_period;
    if(!options.isEmpty())
        d->context->setOptions(options);

    CuFormulaParseHelper ph;
    QString src = ph.injectHost(d->host, s);
    CuControlsReaderA * r = d->context->replace_reader(src.toStdString(), this);
    d->read_ok = (r != nullptr);
    if(r) {
        r->setSource(src);
        // no host in source()
        d->source = s;
        d->formula_is_identity = ph.identityFunction(d->source);
    }
}

void BotReader::unsetSource()
{
    d->context->disposeReader();
}

/**
 * @brief BotReader::setFormula set or change the formula
 * @param formula the new formula
 */
void BotReader::setFormula(const QString &formula)
{
    if(formula != d->command) {
        const QString old_f = d->command;
        d->command = formula;
        emit formulaChanged(d->user_id, d->chat_id, source(), old_f, formula, d->host);
    }
}

void BotReader::m_configure(const CuData& da)
{
    d->unit = QString::fromStdString(da["display_unit"].toString());
    d->label = QString::fromStdString(da["label"].toString());
    d->desc = QString::fromStdString(da["description"].toString());
    d->min = da["min"].toDouble();
    d->max = da["max"].toDouble();
    d->print_format = QString::fromStdString(da["format"].toString());
}

/**
 * @brief BotReader::onUpdate manages new data from the engine
 *
 * If it is the first call and there is no error condition, startSuccess is emitted.
 * startSuccess signal will contain the user and chat ids,
 * the source name (host name excluded) and the formula.
 *
 * @param da
 */
void BotReader::onUpdate(const CuData &da)
{
//    printf("\e[1;33mBotReader.onUpdate: data %s\e[0m\n", da.toString().c_str());
    d->read_ok = !da["err"].toBool();
    if(d->read_ok && d->refresh_cnt == 0) {
        m_check_or_setStartedNow(); // read method comments
        emit startSuccess(d->user_id, d->chat_id, source(), d->command);
    }

    // configure object if the type of received data is "property"
    if(d->read_ok && d->auto_configure && da["type"].toString() == "property") {
        m_configure(da);
    }
    if(da["mode"].toString().size() > 0 && da.containsKey("value")) {
        // at regime, after "property" data check and notify refresh mode
        m_checkRefreshModeAndNotify(da["mode"].toString());
    }
    // in case of error: quit
    // in case we got a value: quit
    // if monitor, m_publishResult is always called
    if(da.containsKey("value") || !d->read_ok ) {
        // evaluate formula, if present, and emit newData
        bool success = m_publishResult(da);
        if(!d->monitor || !success) {
            deleteLater();
        }
    }
    d->refresh_cnt++;
    if(!d->startDateTime.isValid() || d->startDateTime.secsTo(QDateTime::currentDateTime()) >= d->ttl)
        emit lastUpdate(d->chat_id, da);
}

bool BotReader::m_publishResult(const CuData &da)
{
    CuData data(da);
    bool success = !da["err"].toBool();
    // true if value changes
    bool value_change;
     // true if new quality is invalid while old was not or vice versa
    bool quality_invalid_change; // evaluates only the Invalid flag
    bool quality_change; // true if quality changes
    bool notify = false;
    bool is_alert = (d->priority == BotReader::High);
    data["silent"] = (d->priority == Low);
    data["index"] = d->index;
    data["print_format"] = d->print_format.toStdString();
    data["display_unit"] = d->unit.toStdString();
    data["command"] = d->command.toStdString();
    data["msg"] = FormulaHelper().escape(QString::fromStdString(da["msg"].toString())).toStdString();
    CuDataQuality read_quality(da["quality"].toInt());
    if(!success) {
        read_quality.set(CuDataQuality::Invalid);
    }

    // extract value
    CuVariant v = da["value"];
    value_change = (v != d->old_value);
    quality_invalid_change = (read_quality.toInt() & CuDataQuality::Invalid )
            != (d->old_quality.toInt() & CuDataQuality::Invalid);
    quality_change = read_quality != d->old_quality;

//    printf("\e[1;33mBotReader \"%s\" value changed %d cuz old value %s new %s old q %d new q %d\e[0m\n",
//           da["src"].toString().c_str(), value_change, v.toString().c_str(), d->old_value.toString().c_str(),
//           d->old_quality.toInt(), read_quality.toInt());

    if(!d->formula_is_identity) { // dealing with formula
        // data contains the result of a formula true / false or the result of a formula as some value
        bool is_bool = v.getType() == CuVariant::Boolean && v.getFormat() == CuVariant::Scalar;
        //
        // set notify true by default when there is a change and we're dealing with formulas
        notify = true;
        if(is_bool)
            v.toBool() ? data["evaluation"] = std::string("yes") : data["evaluation"] = std::string("no");
        if(d->monitor && is_bool) { // formula evaluates to boolean
            // (e.g. test/device/1/double_scalar > 250 evaluates to bool)
            // monitor type: "monitor" or "alert"
            // "alert": notify on when value goes from false to true only
            // "monitor": notify (silently, with low pri) always when value changes
            notify = quality_invalid_change // 1. always notify if quality turns to invalid or goes back to valid
                    // 2 alert: notify if formula evaluates to true (v.toBool) and before was false (value_change)
                    || (d->priority == BotReader::High && value_change && v.toBool())
                    // 3. monitor: notify if value changed
                    || (d->priority == BotReader::Low && value_change);
        }
        else if(d->monitor && !is_bool) {
            // formula result is the result of a calculation, does not evaluate to bool
            // so publish the result if either
            // - "monitor" priority Low (silent message)
            // or
            // - "alert" priority Normal (message + notification): only on quality changed
            //
            notify = !is_alert || (is_alert && d->old_quality != read_quality);
        }
    }
    else if(value_change) { // no formula
        //
        // simple monitors notify on every value change
        // alerts notify only on quality change
        notify = !is_alert || (is_alert && d->old_quality != read_quality);
    }

    if(value_change)
        d->old_value = v;
    if(quality_change)
        d->old_quality = read_quality;

    if(notify) {
        emit newData(d->chat_id, data);
        d->notify_cnt++;
    }
    return success;
}

void BotReader::m_check_or_setStartedNow()
{
    if(!d->startDateTime.isValid()) // check if not already set
        d->startDateTime = QDateTime::currentDateTime();
}

void BotReader::m_checkRefreshModeAndNotify(const string &refMode)
{
    RefreshMode old_mode = d->refresh_mode;
    if(refMode == "event") d->refresh_mode = Event;
    else if(refMode == "polled") d->refresh_mode = Polled;
    else d->refresh_mode = RefreshModeUndefined;
    if(old_mode != d->refresh_mode) {
        emit modeChanged(d->refresh_mode);
    }
}
