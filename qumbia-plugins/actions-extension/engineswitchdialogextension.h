#ifndef ENGINESWITCHDIALOGEXTENSION_H
#define ENGINESWITCHDIALOGEXTENSION_H

#include <QWidget>
#include <quaction-extension-plugininterface.h>

class EngineSwitchDialogExtensionPrivate;

class EngineSwitchDialogExtension : public QObject,  public QuActionExtensionI
{
    Q_OBJECT
public:
    explicit EngineSwitchDialogExtension(const CuContextI *ctx, QWidget* parent = nullptr);
    virtual ~EngineSwitchDialogExtension();

signals:


    // QuActionExtensionI interface
public:
    QString getName() const;
    CuData execute(const CuData &in, const CuContextI *ctx);
    std::vector<CuData> execute(const std::vector<CuData> &in_list, const CuContextI *ctx);
    QObject *get_qobject();
    const CuContextI *getContextI() const;
    std::string message() const;
    bool error() const;

private:
    EngineSwitchDialogExtensionPrivate *d;

private slots:
    void m_dialog_destroyed(QObject *o);
};

#endif // ENGINESWITCHDIALOGEXTENSION_H
