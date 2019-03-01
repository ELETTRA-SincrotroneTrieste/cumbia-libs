#include "cuformulaplugin.h"
#include "cuformula.h"
#include "cuformulaparser.h"
#include <cucontrolsfactorypool.h>
#include <cumbiapool.h>
#include <QMap>
#include <QString>

class CuFormulasPluginPrivate {
  public:
    CumbiaFormula *cu_f;
    std::string msg;
    bool err;
};

CuFormulaPlugin::CuFormulaPlugin(QObject *parent) :
    QObject(parent), CuFormulaPluginI()
{
    d = new CuFormulasPluginPrivate;
    d->cu_f = new CumbiaFormula();
    d->err = false;
}

CuFormulaPlugin::~CuFormulaPlugin()
{
    pdelete("~CuFormulaPlugin %p", this);
    delete d->cu_f;
    delete d;
}

QString CuFormulaPlugin::getName() const
{
    return "CuFormulasPlugin";
}

Cumbia *CuFormulaPlugin::getCumbia() const
{
    return d->cu_f;
}

CuControlsReaderFactoryI *CuFormulaPlugin::getFormulaReaderFactory(CumbiaPool *cu_poo, const CuControlsFactoryPool &fpool) const
{
    return new CuFormulaReaderFactory(cu_poo, fpool);
}

std::string CuFormulaPlugin::message() const
{
    return d->msg;
}

bool CuFormulaPlugin::error() const
{
    return d->err;
}

std::vector<std::string> CuFormulaPlugin::srcPatterns() const
{
    std::vector<std::string> patts;
    patts.push_back("formula://");
    return patts;
}

bool CuFormulaPlugin::formulaIsNormalized(const QString &formula) const
{
    return CuFormulaParser().isNormalized(formula);
}

QString CuFormulaPlugin::normalizedFormulaPattern() const
{
    return CuFormulaParser().normalizedFormulaPattern();
}



#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(cuformulasplugin, CuFormulasPlugin)
#endif // QT_VERSION < 0x050000
