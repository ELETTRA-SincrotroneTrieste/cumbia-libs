#ifndef CUFORMULASPLUGIN_H
#define CUFORMULASPLUGIN_H

#include <QGenericPlugin>
#include <QObject>
#include <cuformulaplugininterface.h>
#include <cuformulareader.h>

class CuControlsReaderFactoryI;
class CuFormulasPluginPrivate;

class CuFormulaPlugin : public QObject,  public CuFormulaPluginI
{
    Q_OBJECT

#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QGenericPluginFactoryInterface" FILE "cuformulasplugin.json")
#endif // QT_VERSION >= 0x050000

    Q_INTERFACES(CuFormulaPluginI)

public:
    CuFormulaPlugin(QObject *parent = nullptr);

    virtual ~CuFormulaPlugin();

    // QuActionExtensionPluginInterface interface
public:


private:
    CuFormulasPluginPrivate *d;

public:

    // CuFormulasPluginI interface
public:
    QString getName() const;
    Cumbia *getCumbia() const;
    void initialize(CumbiaPool *cu_poo, CuControlsFactoryPool &fpool);
    CuControlsReaderFactoryI* getFormulaReaderFactory() const;
    std::string message() const;
    bool error() const;
    std::vector<std::string> srcPatterns() const;
    void addSrcPattern(const std::string& pattern);
    CuFormulaParserI *getFormulaParserInstance() const;
};

#endif // CUFORMULASPLUGIN_H
