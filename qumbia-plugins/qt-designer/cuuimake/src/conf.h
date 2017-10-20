#ifndef CONF_H
#define CONF_H

#include <QMap>
#include <QString>

class Conf
{
public:
    Conf(const QString& fac);

    void add(const QString& param);

    void add(const QString& orig, const QString& _new);

    QString factory;

    QMap<QString, QString> subs;

    QStringList adds;

};

#endif // CONF_H
