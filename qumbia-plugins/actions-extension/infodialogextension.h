#ifndef INFODIALOGEXTENSION_H
#define INFODIALOGEXTENSION_H

#include <QObject>
#include <quaction-extension-plugininterface.h>

class CuContextI;
class InfoDialogExtensionPrivate;

class InfoDialogExtension : public QObject,  public QuActionExtensionI
{
    Q_OBJECT
public:
    explicit InfoDialogExtension(const CuContextI *ctx, QObject* parent = NULL);

    virtual ~InfoDialogExtension();

signals:

public slots:

    // QuActionExtensionI interface
public:
    QString getName() const;
    CuData execute(const CuData &in, const CuContextI *ctxI);
    std::vector<CuData> execute(const std::vector<CuData> &in_list, const CuContextI *ctxi);
    QObject *get_qobject();
    const CuContextI *getContextI() const;
    std::string message() const;
    bool error() const;

private:
    InfoDialogExtensionPrivate *d;

};

#endif // INFODIALOGEXTENSION_H
