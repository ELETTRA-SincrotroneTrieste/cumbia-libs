#include "simpleformulaeval.h"
#include <cudata.h>
#include <QJSValue>
#include <QJSEngine>
#include <cuformulareader.h>
#include <cudataquality.h>
#include <QDateTime>

class SimpleFormulaEvalPrivate {
public:
    QString formula;
    CuData result;
};


SimpleFormulaEval::SimpleFormulaEval(QObject *parent, const QString &formula) : QThread(parent)
{
    QJSEngine sen;
    d = new SimpleFormulaEvalPrivate;
    d->formula = formula;
    connect(this, SIGNAL(finished()), this, SLOT(publishResult()));
}

void SimpleFormulaEval::run()
{
    QJSEngine sen;
    QJSValue sval = sen.evaluate(d->formula);
    bool err = sval.isError();
    CuDataQuality dq(CuDataQuality::Undefined);
    d->result[CuDType::Src] = d->formula.toStdString();  // result["src"]
    d->result[CuDType::Err] = err;  // result["err"]
    if(err) {
        d->result[CuDType::Message] = QString("SimpleFormulaEval: evaluation error: %1").arg(d->formula).toStdString();  // result["msg"]
        dq.set(CuDataQuality::Invalid);
    }
    d->result["formula"] = d->formula.toStdString();
    d->result[CuDType::Time_ms] = static_cast<long int>(QDateTime::currentMSecsSinceEpoch());  // result["timestamp_ms"]
    if(!err) {
        CuVariant val = qobject_cast<CuFormulaReader *>(parent())->fromScriptValue(sval);
        val.getFormat() == CuVariant::Scalar ? d->result[CuDType::DataFormatStr] = "scalar" :  // result["dfs"]
                d->result[CuDType::DataFormatStr] = "vector";  // result["dfs"]
        d->result[CuDType::Value] = val;  // result["value"]
        dq.set(CuDataQuality::Valid);
    }
    d->result[CuDType::Quality] = dq.toInt();  // result["q"]
    d->result[CuDType::QualityColor] = dq.color();  // result["qc"]
    d->result[CuDType::QualityString] = dq.name();  // result["qs"]
}

void SimpleFormulaEval::publishResult()
{
    emit onCalcReady(d->result);
    deleteLater();
}
