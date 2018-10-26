#ifndef GETTDBPROPERTYEXTENSION_H
#define GETTDBPROPERTYEXTENSION_H

#include <QObject>
#include <quaction-extension-plugininterface.h>

class CuContext;

class GetTDbPropertyExtensionPrivate;

class GetTDbPropertyExtension : public QObject, public QuActionExtensionI
{
    Q_OBJECT
public:

    GetTDbPropertyExtension(CuContext *ctx, QObject* parent = NULL);

    virtual ~GetTDbPropertyExtension();

    // QuActionExtensionI interface
public:
    QString getName() const;

    CuData execute(const CuData &in);

    QObject* get_qobject();

signals:
    void onDataReady(const CuData& da);

private:
    GetTDbPropertyExtensionPrivate *d;
};

#endif // GETTDBPROPERTYEXTENSION_H
