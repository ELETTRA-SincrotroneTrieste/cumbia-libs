#include "curndfunctiongenerators.h"
#include <vector>
#include <time.h>
#include <qalgorithms.h>
#include <math.h>
#include <QTextStream>
#include <QFile>
#include <QScriptEngine>
#include <QScriptValue>
#include <QRandomGenerator>
#include <QtDebug>

void CuRndRandomFunctionGen::configure(const CuData &options)
{
    extractConf(options);
}

void CuRndRandomFunctionGen::generate(CuData &res)
{
    QRandomGenerator *rg = QRandomGenerator::global();
    res["function"] = "random";
    if(data->size > 1) {
        std::vector<double> vd;
        for(int i = 0; i < data->size; i++) {
            vd.push_back(data->min + rg->generateDouble() * (data->max - data->min));
        }
        res["value"] = vd;
    }
    else {
        res["value"] = data->min + rg->generateDouble() * (data->max - data->min);
    }
    res["err"] = false;
    res["msg"] = "successfully generated " + std::to_string(data->size) + " random number(s)";
}

CuRndFunctionGenA::Type CuRndRandomFunctionGen::getType() const
{
    return CuRndFunctionGenA::RandomF;
}

void CuRndSinFunctionGen::configure(const CuData &opts)
{
    extractConf(opts);
    options = opts;
}

void CuRndSinFunctionGen::generate(CuData &res) {
    time_t tp;
    time(&tp);
    qsrand(tp);
    double amplitude = static_cast<double>(data->min + qrand() % qRound(data->max));
    double angle = qrand() % 360;
    angle = 2 * M_PI * angle / 360.0;
    res["function"] = "random_sin";
    if(data->size == 1) {
        res["value"] = amplitude * sin(angle);
        res["msg"] = "generated a random amplitude sin with a random angle between 0 and 360 degrees";
    }
    else {
        bool ok = false;
        double step = 0.01; // rad
        if(options.containsKey("step"))
            step = options["step"].toDouble(&ok);
        if(!ok)
            step = data->size / 2.0 / M_PI;
        std::vector<double> vd;
        for(int i = 0; i < data->size; i++) {
            vd.push_back(amplitude * sin(step * i));
        }
        res["value"] = vd;
        res["msg"] = "generated a " + std::to_string(data->size) + " points sin spectrum with step " +
                std::to_string(step);
    }
    res["err"] = false;
}

CuRndFunctionGenA::Type CuRndSinFunctionGen::getType() const
{
    return CuRndRandomFunctionGen::SinF;
}

CuRndJsFunctionGen::CuRndJsFunctionGen(const QString &filenam) {
    m_call_cnt = 0;
    QFile jsf(filenam);
    m_filenam = filenam;
    if(jsf.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QTextStream in(&jsf);
        m_jscode = QString("(%1)").arg(in.readAll());
        jsf.close();
    }
    else
        m_error = jsf.errorString();
    m_jse = new QScriptEngine();
    m_last_result = m_jse->newObject();
}

void CuRndJsFunctionGen::configure(const CuData &options)
{
    extractConf(options);
    m_options = options;
}

/*!
 * \brief Executes the function in the JavaScript file specified in the class constructor
 * \param res partially filled data that will store the result of the function in the
 *        "value" value.
 *
 * \par Input arguments
 * The following input arguments will be passed to the JavaScript function, in this exact order.
 * The arguments are stored in the options within the configure method.
 *
 * \li size  (the size of data to be generated: scalar or spectrum with the given size)
 * \li min the minimum value expected from the function
 * \li max the maximum value expected from the function
 * \li period the interval in milliseconds between subsequent function calls
 * \li call count, the number of times generate has been called so far
 * \li last result, the last valid value obtained from the previous call
 *
 * \par Output from the JavaScript function
 * The result from the JavaScript function is stored into the *value* value within the
 * input CuData argument passed as (*non const*) reference.
 *
 * If an error occurs, *err* and *msg* values will be set accordingly
 */
void CuRndJsFunctionGen::generate(CuData &res) {
    if(m_error.isEmpty()) {
        double min, max;
        int size, period;
        m_options["min"].to<double>(min);
        m_options["max"].to<double>(max);
        m_options["size"].to<int>(size);
        m_options["period"].to<int>(period);
        ++m_call_cnt;
        QScriptValue val;
        QScriptValue callable = m_jse->evaluate(m_jscode, m_filenam);
        res["err"] = callable.isError();
        if(callable.isError() || !callable.isFunction()) {
            res["msg"] = std::string("CuRndJsFunctionGen::generate: error evaluating JS function or not callable: line ")
                    + "file: " + callable.property("fileName").toString().toStdString() +
                    ": " + std::to_string(callable.property("lineNumber").toInt32()) + ": " +
                    callable.toString().toStdString();
            res["err"] = true;
        }
        else {
            QScriptValueList args;
            args << size << min << max << period << m_call_cnt;
            val = callable.call(QScriptValue(), args);
            res["err"] = val.isError();
            if(val.isError()) {
                res["msg"] = std::string("CuRndJsFunctionGen::generate: error calling JS function: \"" +
                                         val.toString().toStdString() +
                                         "\" file \"" + val.property("fileName").toString().toStdString() +
                                         "\" line " + std::to_string(val.property("lineNumber").toInt32()));
            }
            // extract value
            else if(val.isArray()) {
                bool array_type_error = false;
                std::vector<double> vd;
                std::vector<std::string> vs;
                std::vector<bool> vb;
                quint32 len = val.property("length").toUInt32(); // toInt returns quint32!
                for(quint32 i = 0; i < len && !array_type_error; i++) {
                    QScriptValue v = val.property(i);
                    if(v.isBool())
                        vb.push_back(v.toBool());
                    if(v.isNumber())
                        vd.push_back(v.toNumber());
                    else if(v.isString())
                        vs.push_back(v.toString().toStdString());
                    else array_type_error = true;
                }
                if(len == 0)
                    res["value"] = vd;
                else if(vd.size() > 0)
                    res["value"] = vd;
                else if(vs.size() > 0)
                    res["value"] = vs;
                else if(vb.size() > 0)
                    res["value"] = vb;
            }
            else if(val.isBool())
                res["value"] = val.toBool();
            else if(val.isNumber())
                res["value"] = val.toNumber();
            else if(val.isString())
                res["value"] = val.toString().toStdString();
            else {
                res["err"] = true;
                res["msg"] = "CuRndJsFunctionGen::generate js value \"" + val.toString().toStdString() + "\" returned from the function "                                                                                                                                   " nor a type string";
            }
        }
        if(!res["err"].toBool())
            m_last_result = val;
    }
    else {
        res["err"] = true;
        res["msg"] = std::string("error opening file: \"") + m_filenam.toStdString() +
                "\":" + m_error.toStdString();
    }
}

CuRndFunctionGenA::Type CuRndJsFunctionGen::getType() const {
    return CuRndFunctionGenA::FromJSFileF;
}
