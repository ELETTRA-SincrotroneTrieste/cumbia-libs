#include "curndfunctiongenerators.h"
#include <vector>
#include <time.h>
#include <qalgorithms.h>
#include <math.h>
#include <QTextStream>
#include <QFile>
#include <QJSEngine>
#include <QRandomGenerator>

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
    m_jsfilenam = filenam;
}

void CuRndJsFunctionGen::configure(const CuData &options)
{
    extractConf(options);
}

void CuRndJsFunctionGen::generate(CuData &res) {
    QFile jsf(m_jsfilenam);
    if(jsf.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QTextStream in(&jsf);
        QString f = in.readAll();
        QJSEngine jse;
        QJSValue val = jse.evaluate(f, m_jsfilenam);
        res["err"] = val.isError();
        if(val.isError()) {
            res["msg"] = std::string("CuRndJsFunctionGen::generate: error evaluating JS function: line ")
                    + "file: " + val.property("fileName").toString().toStdString() + ": " + std::to_string(val.property("lineNumber").toInt()) + ": " +
                    val.toString().toStdString();
        }
        else {
            // extract value
            if(val.isArray() && val.isNumber()) {
                std::vector<double> vd;
                const int len = val.property("length").toInt();
                for(int i = 0; i < len; i++)
                    vd.push_back(val.property(i).toNumber());
                res["value"] = vd;
            }
            else if(val.isArray() && val.isString()) {
                std::vector<std::string> vs;
                const int len = val.property("length").toInt();
                for(int i = 0; i < len; i++)
                    vs.push_back(val.property(i).toString().toStdString());
                res["value"] = vs;
            }
            else if(val.isNumber()) {
                res["value"] = val.toNumber();
            }
            else if(val.isString())
                res["value"] = val.toString().toStdString();
            else {
                res["err"] = true;
                res["msg"] = "CuRndJsFunctionGen::generate js value returned from the function "
                        "in file \"" + m_jsfilenam.toStdString() +  "\" is neither of type number"
                        " nor a type string";
            }
        }
    }
    else {
        res["err"] = true;
        res["msg"] = std::string("error opening file: \"") + m_jsfilenam.toStdString() +
                "\":" + jsf.errorString().toStdString();
    }
}

CuRndFunctionGenA::Type CuRndJsFunctionGen::getType() const {
    return CuRndFunctionGenA::FromJSFileF;
}
