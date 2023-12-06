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
    CuFormulaReaderFactory* reader_f;
    std::string msg;
    bool err;
    std::vector<std::string> patterns;
};

CuFormulaPlugin::CuFormulaPlugin(QObject *parent) :
    QObject(parent), CuFormulaPluginI()
{
    d = new CuFormulasPluginPrivate;
    d->cu_f = new CumbiaFormula();
    d->err = false;
    d->patterns.push_back("formula://");
    d->reader_f = nullptr;
}

CuFormulaPlugin::~CuFormulaPlugin()
{
    pdelete("~CuFormulaPlugin %p deleting cumbia %p", this, d->cu_f);
    delete d->cu_f;
    if(d->reader_f)
        delete d->reader_f;
    delete d;
}

QString CuFormulaPlugin::getName() const
{
    return "CuFormulaPlugin";
}

Cumbia *CuFormulaPlugin::getCumbia() const
{
    return d->cu_f;
}

/*!
 * \brief plugin is initialized with application wide CumbiaPool and CuControlsFactoryPool
 * \param cu_poo the application wide pointer to CumbiaPool
 * \param fpool the application wide (non const) reference to CuControlFactoryPool
 *
 * After plugin initialization, both pools will have the *formula* domain registered with the *cuformula*
 * patterns (formula:// prefix).
 *
 * \par Important note 1
 * CuFormulaPlugin::initialize shall be called before any source setup, so that the application wide
 * pools will contain the *formula* domain with the necessary factories.
 *
 * \par Important note 2
 * cu_poo and fpool *shall be initialized with all other desired engines before* this call.
 */
void CuFormulaPlugin::initialize(CumbiaPool *cu_poo, CuControlsFactoryPool &fpool)
{
    d->reader_f = new CuFormulaReaderFactory(cu_poo, fpool);
    cu_poo->registerCumbiaImpl("formula", d->cu_f);
    fpool.registerImpl("formula", *d->reader_f);
    fpool.setSrcPatterns("formula", d->patterns);
    cu_poo->setSrcPatterns("formula", d->patterns);
}

CuControlsReaderFactoryI *CuFormulaPlugin::getFormulaReaderFactory() const
{
    return d->reader_f;
}

std::string CuFormulaPlugin::message() const
{
    return d->msg;
}

bool CuFormulaPlugin::error() const
{
    return d->err;
}

/**
 * @brief CuFormulaPlugin::srcPatterns return the list of regular expression patterns defining a
 *        CuFormulaPlugin *source* pattern.
 *
 * @return the list of patterns
 *
 * \par Default pattern
 * The default pattern for a CuFormulaPlugin *source* is *formula:// *
 */
std::vector<std::string> CuFormulaPlugin::srcPatterns() const
{
    return d->patterns;
}

/**
 * @brief CuFormulaPlugin::addSrcPattern add a regular expression pattern to the list of patterns
 * @param pattern a new pattern to be added to the source patterns for the formula plugin
 *
 * \par Note
 * The default pattern is *formula:// *
 */
void CuFormulaPlugin::addSrcPattern(const std::string &pattern)
{
    d->patterns.push_back(pattern);
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
