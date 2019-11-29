#ifndef QUTFINDSRCSPLUGINI_H
#define QUTFINDSRCSPLUGINI_H

#include <QStringList>

class QuTFindSrcsPluginI
{
public:
    virtual ~QuTFindSrcsPluginI() {}

    virtual QStringList matches(const QString& find) = 0;
    virtual QString errorMessage() const = 0;
    virtual bool error() const = 0;
};

#define QuTFindSrcsPluginI_iid "eu.elettra.qutils.QuTFindSrcsPluginI"

Q_DECLARE_INTERFACE(QuTFindSrcsPluginI, QuTFindSrcsPluginI_iid)

#endif // QUTFINDSRCSPLUGINI_H
