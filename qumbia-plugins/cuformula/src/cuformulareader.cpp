#include "cuformulareader.h"
#include "cuformula.h"
#include "cuformulaparser.h"

#include <cucontrolsfactorypool.h>
#include <cumbiapool.h>
#include <cudataquality.h>
#include <quwatcher.h>
#include <cumacros.h>
#include <limits.h>
#include <QScriptEngine>
#include <QtDebug>


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
    printf("set source on formula reader: \"%s\"\n", qstoc(s));
    d->source = s;
    d->formula_parser.parse(s);
    d->values.clear();
    d->qualities.clear();
    d->errors.clear();
    d->messages.clear();

    m_disposeWatchers();
    qDebug() << __PRETTY_FUNCTION__ << "detected sources count" << d->formula_parser.sourcesCount();
    size_t i = 0;
    for(i = 0; i < d->formula_parser.sourcesCount(); i++) {
        const std::string &src = d->formula_parser.source(i);
        QuWatcher *watcher = new QuWatcher(this, d->cu_poo, d->fpoo);
        printf("\e[1;32msource on watcher \"%s\"\e[0m\n", src.c_str());
        connect(watcher, SIGNAL(newData(const CuData&)), this, SLOT(onNewData(const CuData&)));
        watcher->setSource(QString::fromStdString(src));
        // update the i-th source with the free from wildcard source name
        qDebug() << __PRETTY_FUNCTION__ <<  "src" << i << watcher->source();

        d->formula_parser.updateSource(i, watcher->source().toStdString());
        d->values.push_back(CuVariant());
        d->qualities.push_back(CuDataQuality());
        d->errors.push_back(true);
        d->messages.push_back("waiting for " + src);
    }
    qDebug() << __PRETTY_FUNCTION__ <<  "leaving setSource";
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

    CuData dat(da);
    dat["srcs"] = d->formula_parser.sources();
    std::string src = da["src"].toString();
    if(src.size() == 0) {
        d->error = true;
        d->message = "CuFormulaReader.onNewData: input data without \"src\" key";
    }
    else {
        bool err = da["err"].toBool();
        std::string msg = da["msg"].toString();
        long idx = d->formula_parser.indexOf(src);
        if(idx < 0) {
            err = true;
            msg = "CuFormulaReader::onNewData: no source \"" + src + "\" is found";
        }
        if(idx > -1 && da.containsKey("value")) {
            size_t index = static_cast<size_t>(idx);
            err = da["data_format_str"].toString( ) != "scalar";
            if(!err) {
                double val = da["value"].toDouble();
                d->values[index] = val;
                printf("3a1\n");
                CuFormulaParser::State st = d->formula_parser.compile(d->values);
                if(st == CuFormulaParser::ReadingsIncomplete) {
                    msg = "not all readings are ok";
                    err = true;
                    printf("3a2\n");
                }
                else if(st != CuFormulaParser::CompileOk) {
                    err = true;
                    printf("3b1\n");
                    msg = std::string(d->formula_parser.states[st]);
                    printf("3b2\n");
                    if(d->formula_parser.message().length() > 0)
                        msg += ": " + d->formula_parser.message().toStdString();
                }
                else {
                    QScriptEngine se(this);
                    QScriptValue sval = se.evaluate(d->formula_parser.compiledFormula());
                    printf("\e[1;32m1. \e[0mCuFormulaReader.onNewData: %s %f\e[0m\n", src.c_str(), val);
                    printf("\e[1;32m2. \e[0mCuFormulaReader.onNewData: evaluating \e[1;36m%s = %f\e[0m\n\n",
                           qstoc(d->formula_parser.compiledFormula()), sval.toNumber());

                    if(sval.isValid())
                        dat["value"] = sval.toNumber();
                    else
                        dat["value"] = -1;
                }
                printf("4\n");
            }
            else {
                d->values[index] = CuVariant(); // invalidate
                msg = "CuFormulaReader: " + src + " is not a scalar";
                dat["err"] = true;
            }
            d->errors[index] = err;
            d->messages[index] = msg;

            // update quality for index idx: in case of error, Invalid quality
            !err ? d->qualities[index] = dat["quality"].toInt() :
                d->qualities[index] = CuDataQuality(CuDataQuality::Invalid);
        }

        // now combine all qualities together
        CuDataQuality cuq = combinedQuality();
        dat["quality"] = cuq.toInt();
        dat["quality_color"] = cuq.color();
        dat["quality_string"] = cuq.name();
        dat["msg"] = combinedMessage();
        dat["err"] = err;
    }
    d->listener->onUpdate(dat);
}

CuDataQuality CuFormulaReader::combinedQuality() const
{
    CuDataQuality q; // zero: invalid
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
        msg += d->formula_parser.source(i) + ": " + d->messages[i] + "\n";
    }
    return msg;
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
