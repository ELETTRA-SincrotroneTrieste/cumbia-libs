#include "cuformulareader.h"
#include "cuformula.h"
#include "cuformulaparser.h"
#include "simpleformulaeval.h"

#include <cucontrolsfactorypool.h>
#include <cumbiapool.h>
#include <cudataquality.h>
#include <quwatcher.h>
#include <cumacros.h>
#include <limits.h>
#include <QVector>
#include <QScriptEngine>
#include <QtDebug>
#include <QDateTime>
#include <QRegularExpression>

class CuFormulaReaderFactoryPrivate {
public:
    CuData options;
    CumbiaPool *cu_poo;
    CuControlsFactoryPool fpool;
};

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

CuControlsReaderA *CuFormulaReaderFactory::create(Cumbia *c, CuDataListener *l) const
{
    return new CuFormulaReader(c, l, d->cu_poo, d->fpool);
}

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
    std::vector<std::string> messages;
    QScriptEngine scriptEngine;
};

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
}

CuFormulaReader::~CuFormulaReader()
{
    pdelete("~CuFormulaReader %p", this);
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
    d->source = s;
    d->values.clear();
    d->qualities.clear();
    d->errors.clear();
    d->messages.clear();
    d->formula_parser.parse(s);
    qDebug() << __PRETTY_FUNCTION__ << "detected sources count" << d->formula_parser.sourcesCount()
             << "from " << s;

    if(d->formula_parser.error())
        m_notifyFormulaError();
    else if(d->formula_parser.sourcesCount() > 0) {
        m_disposeWatchers();
        size_t i = 0;
        for(i = 0; i < d->formula_parser.sourcesCount(); i++) {
            const std::string &src = d->formula_parser.source(i);
            QuWatcher *watcher = new QuWatcher(this, d->cu_poo, d->fpoo);
            connect(watcher, SIGNAL(newData(const CuData&)), this, SLOT(onNewData(const CuData&)));
            watcher->setSource(QString::fromStdString(src));
            // update the i-th source with the free from wildcard source name
            d->formula_parser.updateSource(i, watcher->source().toStdString());
            d->values.push_back(CuVariant());
            d->qualities.push_back(CuDataQuality(CuDataQuality::Undefined));
            d->errors.push_back(true);
            d->messages.push_back("waiting for " + src);
        }
    }
    else {
        SimpleFormulaEval *sfe = new SimpleFormulaEval(this, d->formula_parser.formula());
        connect(sfe, SIGNAL(onCalcReady(const CuData&)), this, SLOT(onNewData(const CuData&)));
        sfe->start();
    }
    m_srcReplaceWildcards();
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
}

void CuFormulaReader::getData(CuData &d_ino) const
{
}

void CuFormulaReader::onNewData(const CuData &da)
{
    d->error = false;
    d->message.clear();
    std::string src = da["src"].toString();
    bool err = da["err"].toBool();
    bool all_read_once = false;
    if(src.size() == 0) { // will not call onUpdate on listener
        d->error = true;
        d->message = "CuFormulaReader.onNewData: input data without \"src\" key";
    }
    else if(d->formula_parser.sourcesCount() > 0) {
        CuData dat;

        dat["timestamp_ms"] = da["timestamp_ms"];
        dat["timestamp_us"] = da["timestamp_us"];
        dat["srcs"] = d->formula_parser.sources();

        dat["formula"] = d->formula_parser.formula().toStdString();
        std::string msg = da["msg"].toString();
        long idx = d->formula_parser.indexOf(src);
        if(idx < 0) {
            err = true;
            msg = "CuFormulaReader::onNewData: no source \"" + src + "\" is found";
        }
        else if(idx > -1 && err) {
            d->errors[static_cast<size_t>(idx)] = err;
            d->messages[static_cast<size_t>(idx)] = msg;
        }
        else if(!err && da.containsKey("value")) {

            size_t index = static_cast<size_t>(idx);
            if(!err) {
                d->values[index] = da["value"];
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
                    QScriptValue result;
                    QString formula = d->formula_parser.preparedFormula();
                    printf("preparing to evaluate formula pretty %s\e[0m\n", qstoc(formula));
                    QScriptValue sval = d->scriptEngine.evaluate(formula);
                    err = !sval.isFunction();
                    if(err) {
                        msg = QString("CuFormulaReader.onNewData: formula \"%1\" is not a function")
                                .arg(formula).toStdString();
                    }
                    else {
                        QScriptValueList valuelist;
                        for(size_t i = 0; i< d->values.size() && !d->error; i++) {
                            const CuVariant& val = d->values[i];
                            const CuVariant::DataFormat dfo = val.getFormat();
                            QScriptValue v;
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
                        if(!err) {
                            result = sval.call(QScriptValue(), valuelist);
                        }
                    }

                    err = !result.isValid() && !result.isError();

                    if(!err) {
                        CuVariant resvar = fromScriptValue(result);
                        err = !resvar.isValid();
                        if(!err) {
                            dat["value"] = resvar;
                            resvar.getFormat() == CuVariant::Vector ? dat["data_format_str"] = "vector"
                                    : dat["data_format_str"] = "scalar";
                        }
                        else
                            msg = d->message.toStdString(); // set by m_fromScriptValue


//                        printf("\e[1;32m1. \e[0mCuFormulaReader.onNewData: evaluating \e[1;36m%s makes %s\e[0m\n\n",
//                               qstoc(d->formula_parser.formula()), resvar.toString().c_str());
                    }
                    else {
                        msg = "failed to call function " + d->formula_parser.formula().toStdString();
                    }
                } // all read once
            } // if ! err
            else {
                d->values[index] = CuVariant(); // invalidate
                dat["err"] = true;
            }
            d->errors[index] = err;
            d->messages[index] = msg;

            // update quality for index idx: in case of error, Invalid quality
            !err ? d->qualities[index] = da["quality"].toInt() :
                   d->qualities[index] = CuDataQuality(CuDataQuality::Invalid);

        } // if(!err && idx > -1 && da.containsKey("value"))

        // now combine all qualities together
        CuDataQuality cuq = combinedQuality();
        dat["quality"] = cuq.toInt();
        dat["quality_color"] = cuq.color();
        dat["quality_string"] = cuq.name();
        dat["msg"] = combinedMessage();
        // if formula has a name, put it in src
        !d->formula_parser.name().isEmpty() ? dat["src"] = d->formula_parser.name().toStdString() :
            dat["src"] = combinedSources();
        dat["err"] = err;

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

QScriptValue CuFormulaReader::m_getScalarVal(const CuVariant &v)
{
    const CuVariant::DataType dty = v.getType();
    if(dty == CuVariant::Boolean)
        return QScriptValue(v.toBool());
    else if(dty == CuVariant::String)
        return QScriptValue(QString::fromStdString(v.toString()));
    else if(v.isInteger() || v.isFloatingPoint()) {
        double dou;
        v.to<double>(dou);
        return QScriptValue(dou);
    }
    return QScriptValue();
}

QScriptValue CuFormulaReader::m_getVectorVal(const CuVariant &v)
{
    QScriptValue arrayv = d->scriptEngine.newArray(static_cast<uint>(v.getSize()));
    const CuVariant::DataType dty = v.getType();
    if(dty == CuVariant::Boolean) {
        std::vector<bool> bv = v.toBoolVector();
        for(quint32 i = 0; i < bv.size(); i++)
            arrayv.setProperty(i, QScriptValue(bv[i]));
    }
    else if(dty == CuVariant::String) {
        std::vector<std::string> sv = v.toStringVector();
        for(quint32 i = 0; i < sv.size(); i++)
            arrayv.setProperty(i, QScriptValue(QString::fromStdString(sv[i])));
    }
    else if(v.isInteger() || v.isFloatingPoint()) {
        std::vector<double> douve;
        v.toVector<double>(douve);
        for(quint32 i = 0; i < douve.size(); i++) {
            arrayv.setProperty(i, QScriptValue(douve[i]));
        }
    }
    return arrayv;
}

CuVariant CuFormulaReader::fromScriptValue(const QScriptValue &v)
{
    d->error = false;
    if(v.isArray()) {
        CuVariant::DataType dt = CuVariant::TypeInvalid;
        quint32 len = v.property("length").toUInt32();
        std::vector<bool> bv;
        std::vector<std::string> sv;
        std::vector<double> dv;
        for(quint32 i = 0; i < len && !d->error; i++) {
            QScriptValue ith_v = v.property(i);
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

CuVariant::DataType CuFormulaReader::getScriptValueType(const QScriptValue &v) const
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
    da["err"] = true;
    da["msg"] = m.toStdString();
    da["src"] = d->source.toStdString();
    da["timestamp_ms"] = QDateTime::currentMSecsSinceEpoch();
    d->listener->onUpdate(da);
}

void CuFormulaReader::m_srcReplaceWildcards()
{
    size_t cnt = d->formula_parser.sourcesCount();
    std::string srcs;
    for(size_t i = 0; i < cnt; i++) {
        i < cnt - 1 ? srcs += d->formula_parser.source(i) + "," : srcs += d->formula_parser.source(i);
    }
    QString pattern = QString("formula://%1").arg(FORMULA_RE);
    QRegularExpression re(pattern, QRegularExpression::DotMatchesEverythingOption);
    QRegularExpressionMatch match = re.match(d->source);
    if(match.hasMatch() && match.capturedTexts().size() == FORMULA_RE_CAPTURES_CNT) {
        d->source.replace(match.captured(2), QString::fromStdString(srcs));
    }
}
