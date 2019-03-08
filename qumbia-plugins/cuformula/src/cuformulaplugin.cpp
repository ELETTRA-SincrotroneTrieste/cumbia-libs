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
    pdelete("~CuFormulaPlugin %p deleting cumbia %p", this, d->cu_f);
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

/**
 * @brief CuFormulaPlugin::getFormulaParserInstance returns a new instance of CuFormulaParser
 * @return a new CuFormulaParser
 *
 * \par Note
 * The caller acquires the ownership of the new object.
 */
CuFormulaParserI *CuFormulaPlugin::getFormulaParserInstance() const
{
    return new CuFormulaParser();
}



#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(cuformulasplugin, CuFormulasPlugin)
#endif // QT_VERSION < 0x050000
