#include "cuformulareader.h"
#include "cuformulaparser.h"
#include "simpleformulaeval.h"
#include "cuformulautils.h"

#include <cucontrolsfactorypool.h>
#include <cumbiapool.h>
#include <cudataquality.h>
#include <quwatcher.h>
#include <cumacros.h>
#include <limits.h>
#include <cucontext.h>
#include <QVector>
#include <QJSEngine>
#include <QtDebug>
#include <QDateTime>
#include <QRegularExpression>
#include <QCoreApplication>

/*
 * https://doc.qt.io/qt-5/qglobalstatic.html#Q_GLOBAL_STATIC
 *
 * The Q_GLOBAL_STATIC macro creates a type that is necessarily static,
 * at the global scope. It is not possible to place the Q_GLOBAL_STATIC
 * macro inside a function (doing so will result in compilation errors).
 * More importantly, this macro should be placed in source files, never
 * in headers. Since the resulting object is has static linkage, if the
 * macro is placed in a header and included by multiple source files,
 * the object will be defined multiple times and will not cause linking
 * errors. Instead, each translation unit will refer to a different object,
 * which could lead to subtle and hard-to-track errors.
 * The Q_GLOBAL_STATIC macro creates an object that initializes itself on
 * first use in a thread-safe manner: if multiple threads attempt to initialize
 * the object at the same time, only one thread will proceed to initialize,
 * while all other threads wait for completion.
 *
 * If the object is created, it will be destroyed at exit-time, similar to
 * the C atexit function. On most systems, in fact, the destructor will also
 * be called if the library or plugin is unloaded from memory before exit.
 */
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
Q_GLOBAL_STATIC_WITH_ARGS(QRegularExpression, formula_re, (QString("formula://%1").arg(FORMULA_RE)));
#else // deprecated in qt6
Q_GLOBAL_STATIC(QRegularExpression, formula_re, (QString("formula://%1").arg(FORMULA_RE)));
#endif

class CuFormulaReaderFactoryPrivate {
public:
    CuData options;
    CumbiaPool *cu_poo;
    CuControlsFactoryPool fpool;
};

/**
 * @brief CuFormulaReaderFactory::CuFormulaReaderFactory class constructor
 * @param cu_poo a pointer to a valid CumbiaPool
 * @param fpool a const reference to CuControlsFactoryPool
 */
CuFormulaReaderFactory::CuFormulaReaderFactory(CumbiaPool *cu_poo, const CuControlsFactoryPool &fpool)
{
    d = new CuFormulaReaderFactoryPrivate;
    d->cu_poo = cu_poo;
    d->fpool = fpool;
}

CuFormulaReaderFactory::~CuFormulaReaderFactory()
{
    delete d;
}

/** \brief  create a new CuFormulaReader and forward options set on the factory by the
 * CuFormulaReaderFactory client
 *
 * @param c a reference to a Cumbia implementation
 * @param l a CuDataListener
 *
 * @return a new CuFormulaReader
 *
 * \par Options
 * Options set on the CuControlsReaderFactoryI by the client are forwarded to the formula
 * reader. CuFormulaReader employs QuWatcher to perform readings and options will be then
 * sent to the watchers.
 */
CuControlsReaderA *CuFormulaReaderFactory::create(Cumbia *c, CuDataListener *l) const
{
    CuFormulaReader *r = new CuFormulaReader(c, l, d->cu_poo, d->fpool);
    r->setOptions(this->getOptions());
    return r;
}

/**
 * @brief CuFormulaReaderFactory::setOptions set options to be sent to the readers (QuWatcher)
 * @param o CuData with key/value pairs
 *
 * Options will be forwarded to the readers (CuFormulaReader employs QuWatcher)
 */
void CuFormulaReaderFactory::setOptions(const CuData &o)
{
    d->options = o;
}

CuData CuFormulaReaderFactory::getOptions() const
{
    return d->options;
}

CuControlsReaderFactoryI *CuFormulaReaderFactory::clone() const
{
    CuFormulaReaderFactory *f = new CuFormulaReaderFactory(d->cu_poo, d->fpool);
    f->d->options = d->options;
    return f;
}

// end of factory

class CuFormulaReaderPrivate {
public:
    QString source;
    CuDataListener *listener;
    Cumbia *cu_f;
    CumbiaPool *cu_poo;
    CuControlsFactoryPool fpoo;
    CuFormulaParser formula_parser;
    bool error;
    QString message;
    std::vector<CuVariant> values;
    std::vector<CuDataQuality> qualities;
    std::vector<bool> errors;
    std::vector<CuFormulaReader::RefreshMode> modes; // true: event false: polled
    std::vector<std::string> messages;
    std::string display_unit;
    bool single_display_unit;
    QJSEngine jsEngine;
    CuData options;
};

/**
 * @brief SetSrcFailedEvent::SetSrcFailedEvent QEvent that is posted when a setSource on a QuWatcher fails
 * @param src the source name
 * @param msg the message
 */
SetSrcFailedEvent::SetSrcFailedEvent(const QString &src, const QString &msg) :
    QEvent(static_cast<QEvent::Type>(SetSrcFailedType))
{
    source = src;
    message = msg;
}

SetSrcFailedEvent::~SetSrcFailedEvent() {
}

/**
 * @brief CuFormulaReader::CuFormulaReader class constructor.
 * @param c pointer to Cumbia (valid)
 * @param l CuDataListener that will receive updates
 * @param cu_poo pointer to CumbiaPool (valid)
 * @param fpool  a const reference to CuControlsFactoryPool
 *
 * The class is normally instantiated by CuFormulaReaderFactory
 *
 */
CuFormulaReader::CuFormulaReader(Cumbia *c, CuDataListener *l,
                                 CumbiaPool *cu_poo, const CuControlsFactoryPool &fpool)
    : CuControlsReaderA (c, l)
{
    d = new CuFormulaReaderPrivate;
    d->cu_f = c;
    d->listener = l;
    d->cu_poo = cu_poo;
    d->fpoo = fpool;
    d->error = false;
    d->single_display_unit = true;
}

CuFormulaReader::~CuFormulaReader()
{
    pdelete("~CuFormulaReader %p", this);
    if(findChild<SimpleFormulaEval *>()) {
        printf("~CuFormulaReader %p: waiting for simple formula eval... ", this);
        fflush(stdout);
        // findChild<SimpleFormulaEval *>()->wait();
        printf(" [%p] \e[1;32mwaited\e[0m\n", this);
    }
    m_disposeWatchers();
    delete d;
}


/**
 * @brief CuFormulaReader::setSource calls FormulaParser::parse to parse the input string and detect
 *        the sources and the formula. Sources can contain wildcards, so after readers have been set
 *        up, the free from wildcards source is updated on the FormulaParser.
 *
 * @param s The source, in the form
 *
 * \code {source0, source1} (expression(@0, @1)
 * \endcode
 *
 * \code {test/device/1/double_scalar test/device/1->DevDouble(10.1)}  ( (@0+@1) * (@1 -sqrt(@0)))
 * \endcode
 */
void CuFormulaReader::setSource(const QString &s)
{
    CuFormulaUtils fu;
    const QString& src = fu.replaceWildcards(s, qApp->arguments());
    if(d->source != src) {
        printf("CuFormulaReader.setSource: this %p \e[0;36m%s (!= %s)\e[0m\n", this, qstoc(src), qstoc(d->source));
        d->source = src;
        d->values.clear();
        d->qualities.clear();
        d->errors.clear();
        d->messages.clear();
        d->formula_parser.parse(d->source);

        /*if(d->formula_parser.error())
        m_notifyFormulaError();
    else */
        if(!d->formula_parser.error() && d->formula_parser.sourcesCount() > 0) {
            m_disposeWatchers();
            size_t i = 0;
            for(i = 0; i < d->formula_parser.sourcesCount(); i++) {
                const std::string &src = d->formula_parser.source(i);
                QuWatcher *watcher = new QuWatcher(this, d->cu_poo, d->fpoo);
                watcher->getContext()->setOptions(d->options);
                connect(watcher, SIGNAL(newData(const CuData&)), this, SLOT(onNewData(const CuData&)));
                watcher->setSource(QString::fromStdString(src));
                d->values.push_back(CuVariant());
                d->qualities.push_back(CuDataQuality(CuDataQuality::Undefined));
                d->errors.push_back(true);
                d->modes.push_back(RefreshModeUndefined); // polled modes by default
                d->messages.push_back("waiting for " + src);

                if(!watcher->source().isEmpty()) {
                    // update the i-th source with the free from wildcard source name
                    d->formula_parser.updateSource(i, watcher->source().toStdString());
                } // otherwise something went wrong in watcher->setSource (could not guess by src)
                else {
                    QCoreApplication::postEvent(this, new SetSrcFailedEvent(QString::fromStdString(src),
                                                                            m_makeSetSrcError()));
                }
            }
        }
        else {
            SimpleFormulaEval *sfe = new SimpleFormulaEval(this, d->formula_parser.formula());
            connect(sfe, SIGNAL(onCalcReady(const CuData&)), this, SLOT(onNewData(const CuData&)));
            sfe->start();
        }
        m_srcReplaceWildcards();
    }
    else
        perr("CuFormulaReader.setSource: called with the same source %s (== %s)", qstoc(src), qstoc(d->source));
}

QString CuFormulaReader::source() const
{
    return d->source;
}

void CuFormulaReader::unsetSource()
{
}

void CuFormulaReader::sendData(const CuData &d)
{
    // send received data to every single watcher
    foreach(QuWatcher *w, findChildren<QuWatcher *>())
        w->getContext()->sendData(d);
}

void CuFormulaReader::getData(CuData &d_ino) const
{
}

/**
 * @brief CuFormulaReader::setOptions set options on the formula reader
 * @param opt CuData key/value bundle.
 *
 * Options are sent to QuWatcher instances within setSource
 */
void CuFormulaReader::setOptions(const CuData &opt)
{
    d->options = opt;
}

void CuFormulaReader::onNewData(const CuData &da) {
    d->error = false;
    d->message.clear();
    std::string src = da[TTT::Src].toString();  // da["src"]
    bool err = da[TTT::Err].toBool();  // da["err"]
    bool all_read_once = false;
    if(src.size() == 0) { // will not call onUpdate on listener
        d->error = true;
        d->message = "CuFormulaReader.onNewData: input data without \"src\" key";
    }
    else if(d->formula_parser.sourcesCount() > 0) {
        CuData dat;

        dat[TTT::Time_ms] = da[TTT::Time_ms];  // dat["timestamp_ms"], da["timestamp_ms"]
        dat[TTT::Time_us] = da[TTT::Time_us];  // dat["timestamp_us"], da["timestamp_us"]
        dat["srcs"] = d->formula_parser.sources();

        dat["formula"] = d->formula_parser.formula().toStdString();
        std::string msg = da[TTT::Message].toString();  // da["msg"]
        long idx = d->formula_parser.indexOf(src);
        if(idx < 0) {
            err = true;
            msg = "CuFormulaReader::onNewData: no source \"" + src + "\" is found";
        }
        else if(idx > -1 && err) {
            d->errors[static_cast<size_t>(idx)] = err;
            d->messages[static_cast<size_t>(idx)] = msg;
        }
        else if(!err && da.containsKey(TTT::Value)) {  // da.containsKey("value")
            size_t index = static_cast<size_t>(idx);
            if(!err) {
                // refresh mode
                d->modes[index] = m_getRefreshMode(da[TTT::Mode].toString());  // da["mode"]
                d->values[index] = da[TTT::Value];  // da["value"]
                // display unit is stored if homogeneous (or there is a single source)
                if(da.containsKey("display_unit"))
                    m_checkDisplayUnit(da["display_unit"]);
                if(d->display_unit.length() > 0)
                    dat["display_unit"] = d->display_unit;

                all_read_once = m_allValuesValid();
                // CuFormulaParser::ReadingsIncomplete
                if(!all_read_once) {
                    // this is not an error!
                    // unless there is a read error in input parameter CuData
                    // if readings are incomplete (we are still waiting for some
                    // reader) do not call onUpdate on the listener (no need to set msg)
                    //
                    // (see end of method)
                    //
                }
                else {
                    QJSValue result;
                    QString formula = d->formula_parser.preparedFormula();
                    QJSValue sval = d->jsEngine.evaluate(formula);
                    // Returns true if this QJSValue is a function, otherwise returns false.
                    err = !sval.isCallable();
                    if(err) {
                        msg = QString("CuFormulaReader.onNewData: formula \"%1\" is not a function")
                                  .arg(formula).toStdString();
                    }
                    else {
                        QJSValueList valuelist;
                        for(size_t i = 0; i< d->values.size() && !d->error; i++) {
                            const CuVariant& val = d->values[i];
                            const CuVariant::DataFormat dfo = val.getFormat();
                            QJSValue v;
                            if(dfo == CuVariant::Scalar) {
                                v = m_getScalarVal(val);
                            }
                            else if(dfo == CuVariant::Vector) {
                                v = m_getVectorVal(val);
                            }
                            else {
                                err = true;
                                msg = "CuFormulaReader.onNewData: supported formats are scalar and vector";
                            }
                            valuelist << v;
                        }
                        if(!err) { // QJSValue::call see below
                            result = sval.call(valuelist);
                        }
                    }
                    /* QJSValue::call
                     * Calls this QJSValue as a function, passing args as arguments
                     * to the function, and using the globalObject() as the
                     * "this"-object. Returns the value returned from the
                     * function. If this QJSValue is not callable, call()
                     *  does nothing and returns an undefined QJSValue.
                     *  Calling call() can cause an exception to occur in the script
                     *  engine; in that case, call() returns the value that
                     *  was thrown (typically an Error object). You can call
                     *  isError() on the return value to determine whether an exception
                     *  occurred.
                     */
                    err = !result.isUndefined() // QJSValue not callable
                          && !result.isError(); // exception in the engine

                    if(!err) {
                        CuVariant resvar = fromScriptValue(result);
                        err = !resvar.isValid();
                        if(!err) {
                            dat[TTT::Value] = resvar;  // dat["value"]
                            resvar.getFormat() == CuVariant::Vector ? dat[TTT::DataFormatStr] = "vector"  // dat["dfs"]
                                                                    : dat[TTT::DataFormatStr] = "scalar";  // dat["dfs"]
                        }
                        else
                            msg = d->message.toStdString(); // set by m_fromScriptValue
                    }
                    else {
                        msg = "failed to call function " + d->formula_parser.formula().toStdString();
                    }
                } // all read once
            } // if ! err
            else {
                d->values[index] = CuVariant(); // invalidate
                dat[TTT::Err] = true;  // dat["err"]
            }
            d->errors[index] = err;
            d->messages[index] = msg;

            // update quality for index idx: in case of error, Invalid quality
            !err ? d->qualities[index] = da[TTT::Quality].toInt() :  // da["q"]
                d->qualities[index] = CuDataQuality(CuDataQuality::Invalid);

        } // if(!err && idx > -1 && da.containsKey("value"))

        // now combine all qualities together
        CuDataQuality cuq = combinedQuality();
        dat[TTT::Quality] = cuq.toInt();  // dat["q"]
        dat[TTT::QualityColor] = cuq.color();  // dat["qc"]
        dat[TTT::QualityString] = cuq.name();  // dat["qs"]
        dat[TTT::Message] = combinedMessage();  // dat["msg"]
        dat[TTT::Mode] = combinedModes();  // dat["mode"]
        if(d->values.size() == 1) // single source
            dat["raw_value"] = da[TTT::Value];  // da["value"]

        // if formula has a name, put it in src
        !d->formula_parser.name().isEmpty() ? dat[TTT::Src] = d->formula_parser.name().toStdString() :  // dat["src"]
            dat[TTT::Src] = combinedSources();  // dat["src"]
        dat[TTT::Err] = err;  // dat["err"]

        // notify with onUpdate if
        // - error condition (we provide combined quality and combined message)
        // - all sources have been read at least once (d->values is complete)
        //   (we can compute formula only if we have all readings, unless formula
        //   is wrong)
        //
        if(all_read_once || err) {
            d->listener->onUpdate(dat);
        }
    }
    else  { // no sources, maybe simple formula, like "3 + 2"
        d->listener->onUpdate(da);
    }
}

bool CuFormulaReader::event(QEvent *e)
{
    if(e->type() == static_cast<int>(SetSrcFailedEvent::SetSrcFailedType)) {
        SetSrcFailedEvent *srcfe = static_cast<SetSrcFailedEvent *>(e);
        // build a CuData with an error
        CuData err(TTT::Err, true);  // CuData err("err", true)
        err[TTT::Src] = srcfe->source.toStdString();  // err["src"]
        err[TTT::Message] = srcfe->message.toStdString();  // err["msg"]
        onNewData(err);
        e->setAccepted(true);
    }
    return QObject::event(e);
}

CuDataQuality CuFormulaReader::combinedQuality() const
{
    CuDataQuality q(CuDataQuality::Undefined);
    for(size_t  i = 0; i < d->values.size(); i++) {
        if(d->values[i].isNull())
            q.set(CuDataQuality::Invalid);
        else
            q.set(d->qualities[i].toInt());
    }
    return q;
}

std::string CuFormulaReader::combinedMessage() const
{
    std::string msg;
    for(size_t i = 0; i < d->formula_parser.sourcesCount(); i++) {
        msg += d->formula_parser.source(i) + ": " + d->messages[i] +
               + " quality " + d->qualities[i].name() + "\n";
    }
    return msg;
}

std::string CuFormulaReader::combinedSources() const
{
    std::string s;
    const std::vector<std::string> & srcs = d->formula_parser.sources();
    size_t src_cnt = d->formula_parser.sourcesCount();
    for(size_t i = 0; i < src_cnt; i++) {
        i < src_cnt - 1 ? s += srcs[i] + "," : s += srcs[i];
    }
    return s;
}

std::string CuFormulaReader::combinedModes() const
{
    int combinedMode = 0;
    for(size_t i = 0; i < d->modes.size(); i++)
        combinedMode |= d->modes[i];

    if(combinedMode & RefreshModeUndefined)
        return "undefined";
    else if(combinedMode & Polled)
        return "P";
    return "E";
}

std::vector<bool> CuFormulaReader::errors() const
{
    return  d->errors;
}

bool CuFormulaReader::m_allValuesValid() const
{
    for(size_t  i = 0; i < d->values.size(); i++)
        if(d->values[i].isNull())
            return false;
    return true;
}

void CuFormulaReader::m_disposeWatchers()
{
    foreach(QuWatcher *w, findChildren<QuWatcher *>())
        delete w;
}

QJSValue CuFormulaReader::m_getScalarVal(const CuVariant &v)
{
    const CuVariant::DataType dty = v.getType();
    if(dty == CuVariant::Boolean)
        return QJSValue(v.toBool());
    else if(dty == CuVariant::String)
        return QJSValue(QString::fromStdString(v.toString()));
    else if(v.isInteger() || v.isFloatingPoint()) {
        double dou;
        v.to<double>(dou);
        return QJSValue(dou);
    }
    return QJSValue();
}

QJSValue CuFormulaReader::m_getVectorVal(const CuVariant &v)
{
    QJSValue arrayv = d->jsEngine.newArray(static_cast<uint>(v.getSize()));
    const CuVariant::DataType dty = v.getType();
    if(dty == CuVariant::Boolean) {
        std::vector<bool> bv = v.toBoolVector();
        for(quint32 i = 0; i < bv.size(); i++)
            arrayv.setProperty(i, QJSValue(bv[i]));
    }
    else if(dty == CuVariant::String) {
        std::vector<std::string> sv = v.toStringVector();
        for(quint32 i = 0; i < sv.size(); i++)
            arrayv.setProperty(i, QJSValue(QString::fromStdString(sv[i])));
    }
    else if(v.isInteger() || v.isFloatingPoint()) {
        std::vector<double> douve;
        v.toVector<double>(douve);
        for(quint32 i = 0; i < douve.size(); i++) {
            arrayv.setProperty(i, QJSValue(douve[i]));
        }
    }
    return arrayv;
}

CuVariant CuFormulaReader::fromScriptValue(const QJSValue &v)
{
    d->error = false;
    if(v.isArray()) {
        CuVariant::DataType dt = CuVariant::TypeInvalid;
        // quint32 	toUInt(): was toUInt32() in qtscript
        quint32 len = v.property("length").toUInt(); //
        std::vector<bool> bv;
        std::vector<std::string> sv;
        std::vector<double> dv;
        for(quint32 i = 0; i < len && !d->error; i++) {
            QJSValue ith_v = v.property(i);
            if(i == 0) // determine data type from first item
                dt = getScriptValueType(ith_v);

            switch (dt) {
            case CuVariant::Boolean:
                bv.push_back(ith_v.toBool());
                break;
            case CuVariant::Double:
                dv.push_back(ith_v.toNumber());
                break;
            case CuVariant::String:
                sv.push_back(ith_v.toString().toStdString());
                break;
            default:
                d->error = true;
                d->message = "CuFormulaReader.m_fromScriptValue: unsupported data type";
                break;
            } // switch data type
        } // end for
        if(bv.size() > 0)
            return CuVariant(bv);
        else if(sv.size() > 0)
            return CuVariant(sv);
        else if(dv.size() > 0)
            return CuVariant(dv);
    }
    else {
        if(v.isBool())
            return CuVariant(v.toBool());
        else if(v.isString())
            return CuVariant(v.toString().toStdString());
        else if(v.isNumber())
            return CuVariant(static_cast<double>(v.toNumber()));
    }
    d->error = true;
    d->message = "CuFormulaReader::m_fromScriptValue: data conversion error from " + v.toString();
    return CuVariant();
}

CuVariant::DataType CuFormulaReader::getScriptValueType(const QJSValue &v) const
{
    CuVariant::DataType dt = CuVariant::TypeInvalid;
    if(v.isBool())
        dt = CuVariant::Boolean;
    else if(v.isNumber())
        dt = CuVariant::Double;
    else if(v.isString())
        dt = CuVariant::String;
    return dt;
}

void CuFormulaReader::m_notifyFormulaError()
{
    CuData da;
    QString m = "CuFormulaReader: formula error: \"" + d->formula_parser.expression() +"\": " +
                d->formula_parser.message();
    da[TTT::Err] = true;  // da["err"]
    da[TTT::Message] = m.toStdString();  // da["msg"]
    da[TTT::Src] = d->source.toStdString();  // da["src"]
    da[TTT::Time_ms] = QDateTime::currentMSecsSinceEpoch();  // da["timestamp_ms"]
    d->listener->onUpdate(da);
}

void CuFormulaReader::m_srcReplaceWildcards()
{
    size_t cnt = d->formula_parser.sourcesCount();
    std::string srcs;
    for(size_t i = 0; i < cnt; i++) {
        i < cnt - 1 ? srcs += d->formula_parser.source(i) + "," : srcs += d->formula_parser.source(i);
    }
    QRegularExpressionMatch match = formula_re->match(d->source);
    if(match.hasMatch() && match.capturedTexts().size() == FORMULA_RE_CAPTURES_CNT) {
        d->source.replace(match.captured(2), QString::fromStdString(srcs));
    }
}

QString CuFormulaReader::m_makeSetSrcError()
{
    QString msg = "CuFormulaReader: failed to set source.\n"
                  "cumbia could not detect a valid domain for the given source.\n"
                  "Make sure that \"-\" symbols are "
                  "surrounded by spaces if they are minus operators.";
    return msg;
}

// if d->display_unit is empty, set it to the value of the display unit
// if d->display_unit is not empty, reset it to empty if duv is different
// (in case of multiple sources with different display units)
bool CuFormulaReader::m_checkDisplayUnit(const CuVariant &duv)
{
    std::string du;
    if(duv.isValid())
        du = duv.toString();
    if(du.length() > 0 && d->display_unit.length() == 0 && d->single_display_unit)
        d->display_unit = du;
    else if(d->display_unit != du) {
        d->single_display_unit = false;
        d->display_unit.clear();
    }
    return d->single_display_unit;
}

CuFormulaReader::RefreshMode CuFormulaReader::m_getRefreshMode(const std::string &mode) const
{
    if(mode == "E")
        return CuFormulaReader::Event;
    if(mode == "P")
        return CuFormulaReader::Polled;
    if(mode == "oneshot")
        return CuFormulaReader::OneShot;
    return CuFormulaReader::RefreshModeUndefined;
}

