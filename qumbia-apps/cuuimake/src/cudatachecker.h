#ifndef CUDATACHECKER_H
#define CUDATACHECKER_H
#include <QMap>
#include <QRegularExpression>
#include <QString>
#include <QList>

class CuDataChecker
{
public:
    CuDataChecker();
    bool check();
    bool update();

    QMap<QString, QList<QRegularExpression>> subs; // substitutions
    QString msg;
};

#endif // CUDATACHECKER_H
