#ifndef GETTDBPROPERTYEXTENSION_H
#define GETTDBPROPERTYEXTENSION_H

#include <QObject>
#include <quaction-extension-plugininterface.h>
#include <cudatalistener.h>

class CuContext;

class GetTDbPropertyExtensionPrivate;

class GetTDbPropertyExtension : public QObject, public QuActionExtensionI, public CuDataListener
{
    Q_OBJECT
public:

    GetTDbPropertyExtension(const CuContext *ctx, QObject* parent = NULL);

    virtual ~GetTDbPropertyExtension();

    // QuActionExtensionI interface
public:
    QString getName() const;

    CuData execute(const CuData &in, const CuContext *ctx);
    std::vector<CuData> execute(const std::vector<CuData>& in_list, const CuContext *ctx);

    QObject* get_qobject();
    virtual const CuContext *getContext() const;
    std::string message() const;
    bool error() const;

signals:
    void onDataReady(const CuData& da);

private:
    GetTDbPropertyExtensionPrivate *d;

    // CuDataListener interface
public:
    virtual void onUpdate(const CuData &data);
};

#endif // GETTDBPROPERTYEXTENSION_H
