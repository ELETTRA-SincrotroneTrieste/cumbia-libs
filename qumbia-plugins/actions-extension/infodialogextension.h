#ifndef INFODIALOGEXTENSION_H
#define INFODIALOGEXTENSION_H

#include <QObject>
#include <quaction-extension-plugininterface.h>

class CuContext;
class InfoDialogExtensionPrivate;

class InfoDialogExtension : public QObject,  public QuActionExtensionI
{
    Q_OBJECT
public:
    explicit InfoDialogExtension(const CuContext *ctx, QObject* parent = NULL);

    virtual ~InfoDialogExtension();

signals:

public slots:

    // QuActionExtensionI interface
public:
    QString getName() const;
    CuData execute(const CuData &in);
    std::vector<CuData> execute(const std::vector<CuData> &in_list);
    QObject *get_qobject();
    const CuContext *getContext() const;
    std::string message() const;
    bool error() const;

private:
    InfoDialogExtensionPrivate *d;

};

#endif // INFODIALOGEXTENSION_H
