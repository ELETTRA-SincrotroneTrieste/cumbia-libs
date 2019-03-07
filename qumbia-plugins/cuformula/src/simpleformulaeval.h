#ifndef SIMPLEFORMULAEVAL_H
#define SIMPLEFORMULAEVAL_H

#include <QThread>

class CuData;

class SimpleFormulaEvalPrivate;

class SimpleFormulaEval : public QThread {
    Q_OBJECT
public:
    SimpleFormulaEval(QObject *parent, const QString& formula);


protected:
    void run();

protected slots:
    void publishResult();

   signals:
    void onCalcReady(const CuData& da);

private:
    SimpleFormulaEvalPrivate *d;
};

#endif // SIMPLEFORMULAEVEL_H
