#include "simpleformulaeval.h"
#include <cudata.h>
#include <QScriptValue>
#include <QScriptEngine>
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
    QScriptEngine sen;
    d = new SimpleFormulaEvalPrivate;
    d->formula = formula;
    connect(this, SIGNAL(finished()), this, SLOT(publishResult()));
}

void SimpleFormulaEval::run()
{
    QScriptEngine sen;
    QScriptValue sval = sen.evaluate(d->formula);
    bool err = sval.isError() || !sval.isValid();
    CuDataQuality dq(CuDataQuality::Undefined);
    d->result["src"] = d->formula.toStdString();
    d->result["err"] = err;
    if(err) {
        d->result["msg"] = QString("SimpleFormulaEval: evaluation error: %1").arg(d->formula).toStdString();
        dq.set(CuDataQuality::Invalid);
    }
    d->result["formula"] = d->formula.toStdString();
    d->result["timestamp_ms"] = static_cast<long int>(QDateTime::currentMSecsSinceEpoch());
    if(sval.isValid()) {
        CuVariant val = qobject_cast<CuFormulaReader *>(parent())->fromScriptValue(sval);
        val.getFormat() == CuVariant::Scalar ? d->result["dfs"] = "scalar" :
                d->result["dfs"] = "vector";
        d->result["value"] = val;
        dq.set(CuDataQuality::Valid);
    }
    d->result["q"] = dq.toInt();
    d->result["qc"] = dq.color();
    d->result["qs"] = dq.name();
}

void SimpleFormulaEval::publishResult()
{
    emit onCalcReady(d->result);
    deleteLater();
}
