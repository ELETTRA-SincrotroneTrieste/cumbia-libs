#include "botreader.h"
#include "cucontrolsreader_abs.h"
#include <cumacros.h>
#include <cumbiapool.h>
#include <cudata.h>
#include <tsource.h>
#include <QContextMenuEvent>
#include <QMetaProperty>
#include <QScriptEngine>

#include "cucontrolsfactories_i.h"
#include "cucontrolsfactorypool.h"
#include "culinkstats.h"
#include "cucontextmenu.h"
#include "cucontext.h"
#include "formulahelper.h"

/** @private */
class BotReaderPrivate
{
public:
    bool auto_configure;
    bool read_ok;
    bool monitor;
    QString host, source;
    QString formula;
    CuContext *context;
    CuData properties;
    int user_id, chat_id;
    unsigned long refresh_cnt;
    BotReader::Priority priority;
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
                     const QString &formula,
                     Priority pri,
                     const QString &host,
                     bool monitor) :
    QObject(w), CuDataListener()
{
    m_init();
    d->context = new CuContext(cumbia_pool, fpool);
    d->chat_id = chat_id;
    d->user_id = user_id;
    d->priority = pri;
    d->monitor = monitor;
    d->formula = formula;
    d->refresh_cnt = 0;
    const char* env_tg_host = NULL;
    if(host.isEmpty() && (env_tg_host = secure_getenv("TANGO_HOST")) != nullptr) {
        printf("\e[0;33mAUTO DETECTED TANGO HSOT %s\e[0m\n", env_tg_host);
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
    d->read_ok = false;
}

BotReader::~BotReader()
{
    predtmp("\e[1;31m~BotReader %p", this);
    delete d->context;
    delete d;
}

void BotReader::setPropertyEnabled(bool get_props)
{
    d->auto_configure = get_props;
}

QString BotReader::source() const
{
    return d->source; // without host
}

QString BotReader::formula() const
{
    return d->formula;
}

QString BotReader::host() const
{
    return d->host;
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

BotReader::Priority BotReader::priority() const
{
    return d->priority;
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
    if(d->monitor)
        options["period"] = 15000;
    if(!options.isEmpty())
        d->context->setOptions(options);

    QString src(s);
    if(!d->host.isEmpty())
        src = d->host + "/" + src;
    CuControlsReaderA * r = d->context->replace_reader(src.toStdString(), this);
    if(r)
        r->setSource(src);

    // no host in source()
    d->source = s;
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
    if(formula != d->formula) {
        const QString old_f = d->formula;
        d->formula = formula;
        emit formulaChanged(d->chat_id, source(), old_f, formula);
    }
}

void BotReader::m_configure(const CuData& da)
{
    QString description, unit, label;
    CuVariant m, M;

    m = da["min"];  // min value
    M = da["max"];  // max value

    unit = QString::fromStdString(da["display_unit"].toString());
    label = QString::fromStdString(da["label"].toString());

    emit onProperties(d->chat_id, da);
    // save properties
    d->properties = da;
}

void BotReader::onUpdate(const CuData &da)
{
    d->read_ok = !da["err"].toBool();
    if(d->read_ok && d->refresh_cnt == 0)
        emit startSuccess(d->user_id, d->chat_id, source(), d->formula);

    // configure object if the type of received data is "property"
    if(d->read_ok && d->auto_configure && da["type"].toString() == "property") {
        m_configure(da);
    }
    // in case of error: quit
    // in case we got a value: quit
    if(!d->read_ok || da.containsKey("value") || d->monitor) {
        // evaluate formula, if present, and emit newData
        bool success = m_publishResult(da);
        if(!d->monitor || !success) {
            deleteLater();
        }
    }
    d->refresh_cnt++;
}

bool BotReader::m_publishResult(const CuData &da)
{
    CuData data(da);
    bool success = !da["err"].toBool();
    data["silent"] = (d->priority == Low);
    // is there a formula to be evaluated?
    if(!d->formula.isEmpty() && da["data_format_str"].toString() == "scalar") {
        CuVariant v = da["value"];
        double dval; // try to convert value to double in order to apply the formula
        bool ok = v.to<double>(dval);
        if(ok) {
            QString formula(d->formula);
            FormulaHelper fhelp(formula);
            formula = fhelp.replaceWildcards(dval);
            if(fhelp.requiresLeftOperand())
                formula = QString::number(dval) + " " + formula;

            printf("\e[0;36mBotReader.onUpdate: evaluating \"%s\"\e[0m\n", qstoc(formula));
            QScriptEngine eng;
            QScriptValue sv = eng.evaluate(formula);
            data["formula"] = formula.toStdString();
            success = !eng.hasUncaughtException();
            if(!success) {
                data["err"] = true;
                data["msg"] = QString("BotReader: formula \"" + formula + "\" evaluation error: " + sv.toString()).toStdString();
                emit newData(d->chat_id, data);
            }
            else { // formula evaluation successful
                bool is_bool = sv.isBool();
                if(is_bool)
                    sv.toBool() ? data["evaluation"] = std::string("yes") : data["evaluation"] = std::string("no");
                else if(sv.isNumber())
                    data["evaluation"] = sv.toString().toStdString();
                if(!d->monitor) // reply to enquiry in every case
                    emit newData(d->chat_id, data);
                else if(is_bool && sv.toBool()) // formula evaluates to boolean and it is true
                    emit newData(d->chat_id, data);
                else if(!is_bool) {
                    // formula result is the result of a calculation, and does not evaluate to bool
                    // (for instance, test/device/1/double_scalar > 250 evaluates to bool)
                    // so publish the result
                    emit newData(d->chat_id, data);
                }
            } // end formula evaluation successful
        }
    }
    else // no formula
    {
        emit newData(d->chat_id, data);
    }
    return success;
}
