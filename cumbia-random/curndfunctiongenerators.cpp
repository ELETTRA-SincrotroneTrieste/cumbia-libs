#include "curndfunctiongenerators.h"
#include <vector>
#include <time.h>
#include <qalgorithms.h>
#include <math.h>
#include <QTextStream>
#include <QFile>
#include <QJSEngine>
#include <QJSValue>
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
        res[CuDType::Value] = vd;  // res["value"]
    }
    else {
        res[CuDType::Value] = data->min + rg->generateDouble() * (data->max - data->min);  // res["value"]
    }
    res[CuDType::Err] = false;  // res["err"]
    res[CuDType::Message] = "successfully generated " + std::to_string(data->size) + " random number(s)";  // res["msg"]
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
    srand(tp);
    double amplitude = static_cast<double>(data->min + rand() % qRound(data->max));
    double angle = rand() % 360;
    angle = 2 * M_PI * angle / 360.0;
    res["function"] = "random_sin";
    if(data->size == 1) {
        res[CuDType::Value] = amplitude * sin(angle);  // res["value"]
        res[CuDType::Message] = "generated a random amplitude sin with a random angle between 0 and 360 degrees";  // res["msg"]
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
        res[CuDType::Value] = vd;  // res["value"]
        res[CuDType::Message] = "generated a " + std::to_string(data->size) + " points sin spectrum with step " +  // res["msg"]
                std::to_string(step);
    }
    res[CuDType::Err] = false;  // res["err"]
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
    m_jse = new QJSEngine();
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
        m_options[CuDType::Min].to<double>(min);  // m_options["min"]
        m_options[CuDType::Max].to<double>(max);  // m_options["max"]
        m_options["size"].to<int>(size);
        m_options[CuDType::Period].to<int>(period);  // m_options["period"]
        ++m_call_cnt;
        QJSValue val;
        QJSValue callable = m_jse->evaluate(m_jscode, m_filenam);
        res[CuDType::Err] = callable.isError();  // res["err"]
        if(callable.isError() || !callable.isCallable()) {
            res[CuDType::Message] = std::string("CuRndJsFunctionGen::generate: error evaluating JS function or not callable: line ")  // res["msg"]
                    + "file: " + callable.property("fileName").toString().toStdString() +
                    ": " + std::to_string(callable.property("lineNumber").toInt()) + ": " +
                    callable.toString().toStdString();
            res[CuDType::Err] = true;  // res["err"]
        }
        else {
            QJSValueList args;
            args << size << min << max << period << m_call_cnt;
            val = callable.call(args);
            res[CuDType::Err] = val.isError();  // res["err"]
            if(val.isError()) {
                res[CuDType::Message] = std::string("CuRndJsFunctionGen::generate: error calling JS function: \"" +  // res["msg"]
                                         val.toString().toStdString() +
                                         "\" file \"" + val.property("fileName").toString().toStdString() +
                                         "\" line " + std::to_string(val.property("lineNumber").toInt()));
            }
            // extract value
            else if(val.isArray()) {
                bool array_type_error = false;
                std::vector<double> vd;
                std::vector<std::string> vs;
                std::vector<bool> vb;
                quint32 len = val.property("length").toUInt(); // toInt returns quint32!
                for(quint32 i = 0; i < len && !array_type_error; i++) {
                    QJSValue v = val.property(i);
                    if(v.isBool())
                        vb.push_back(v.toBool());
                    if(v.isNumber())
                        vd.push_back(v.toNumber());
                    else if(v.isString())
                        vs.push_back(v.toString().toStdString());
                    else array_type_error = true;
                }
                if(len == 0)
                    res[CuDType::Value] = vd;  // res["value"]
                else if(vd.size() > 0)
                    res[CuDType::Value] = vd;  // res["value"]
                else if(vs.size() > 0)
                    res[CuDType::Value] = vs;  // res["value"]
                else if(vb.size() > 0)
                    res[CuDType::Value] = vb;  // res["value"]
            }
            else if(val.isBool())
                res[CuDType::Value] = val.toBool();  // res["value"]
            else if(val.isNumber())
                res[CuDType::Value] = val.toNumber();  // res["value"]
            else if(val.isString())
                res[CuDType::Value] = val.toString().toStdString();  // res["value"]
            else {
                res[CuDType::Err] = true;  // res["err"]
                res[CuDType::Message] = "CuRndJsFunctionGen::generate js value \"" + val.toString().toStdString() + "\" returned from the function "                                                                                                                                   " nor a type string";  // res["msg"]
            }
        }
        if(!res[CuDType::Err].toBool())  // res["err"]
            m_last_result = val;
    }
    else {
        res[CuDType::Err] = true;  // res["err"]
        res[CuDType::Message] = std::string("error opening file: \"") + m_filenam.toStdString() +  // res["msg"]
                "\":" + m_error.toStdString();
    }
}

CuRndFunctionGenA::Type CuRndJsFunctionGen::getType() const {
    return CuRndFunctionGenA::FromJSFileF;
}
