#ifndef CUFORMULASPLUGIN_INTERFACE_H
#define CUFORMULASPLUGIN_INTERFACE_H

#include <QObject>
#include <QString>
#include <cudata.h>
#include <vector>


//                                       FORMULA REGULAR EXPRESSION PATTERN

// regexp ^(?:\[(.*)\])?\{([\$A-Za-z0-9/,\._\-\:\s&\(\)>]+)\}[\s\n]*(function)?[\s\n]*\({0,1}([a-z,\s]+)\)[\s\n]*(.*)$
// example formula:
// [sum]{test/device/1/double_scalar test/device/1->DevDouble(10.1)} (a,b ){ return a +b; }
// example 2
// {test/device/1/double_scalar test/device/1->DevDouble(10.1)}  function(a, b) { return (a -sqrt(b)); }
// [optional-formula-name]{list,of,sources} function(par1,par2,..,parN) { // javascript function body }
#define FORMULA_RE "^(?:\\[(.*)\\])?\\{([\\$A-Za-z0-9/,\\._\\-\\:\\s&\\(\\)>]+)\\}[\\s\\n]*(function)?[\\s\\n]*" \
    "\\({0,1}([a-z,\\s]+)\\)[\\s\\n]*(.*)$"
#define FORMULA_RE_CAPTURES_CNT 6  // match + 5 captures
#define FORMULA_RE_CAP_NAME 1
#define FORMULA_RE_CAP_SRCS 2
#define FORMULA_RE_CAP_FUN  3
#define FORMULA_RE_CAP_FUN_PAR 4
#define FORMULA_RE_CAP_FUN_BODY 5


// Simplified regular expression
#define SIMPLIFIED_FORMULA_RE "(?:\\[(.*)\\])?\\{(.+)\\}(\\s*function.*)"
#define SIMPLIFIED_FORMULA_RE_CAPTURES_CNT 4  // match + 3 captures
#define SIMPLIFIED_FORMULA_RE_NAME 1
#define SIMPLIFIED_FORMULA_RE_SRCS 2
#define SIMPLIFIED_FORMULA_RE_FUN  3
//
//                                             ------------------------
//

class Cumbia;
class CumbiaPool;
class CuControlsReaderFactoryI;
class QString;
class CuContext;
class CuControlsFactoryPool;

/**
 * @brief The CuFormulaParserI class provides methods to parse an expression, separating the list
 *        of sources that provide data from the formula itself, that combines the values of the
 *        sources to form a result.
 *
 * The plugin implementation employes a *formula parser* for internal usage.
 * It can be employed by the plugin clients to congruously parse expressions.
 */
class CuFormulaParserI {
public:
    virtual ~CuFormulaParserI() {}
    virtual bool parse(const QString& expression) = 0;
    virtual size_t sourcesCount() const = 0;
    virtual std::vector<std::string> sources() const = 0;
    virtual std::string source(size_t i) const = 0;
    virtual std::string joinedSources() const = 0;
    virtual void updateSource(size_t i, const std::string& s) = 0;
    virtual long indexOf(const std::string& src) const = 0;
    /**
     * @brief expression the complete expression, without any processing
     * @return the original expression, with no processing. It usually starts with
     *         formula://{sources,list} and then the formula follows
     */
    virtual QString expression() const = 0;

    /**
     * @brief formula the formula only, separated from the entire expression
     * @return the formula alone, after it is separate from the original expression.
     */
    virtual QString formula() const = 0;

    /**
     * @brief preparedFormula The formula ready to be evaluated by QJSEngine.
     *        The method may remove newlines, extra spaces, and so on
     * @return
     */
    virtual QString preparedFormula() const = 0;
    virtual QString message() const = 0;

    /**
     * @brief name This property can be used to provide a name to the formula.
     *
     * \par Example usage
     * formula://[name]{sources,list} function(a,b) { return a + b; }
     * If *name* is specified within the *optional square brackets* it can be used by
     * cumbia to provide an alternate name for the formula, for example in plot curves.
     * If no name is set, then usually a comma separated list of sources is used.
     * *name* will be set as value with the *src*  key in the result CuData
     *
     * @return
     */
    virtual QString name() const = 0;
    virtual bool error() const = 0;

    /**
     * @brief normalizedFormulaPattern regular expression pattern for a valid normalized formula
     * @return a string that is the regular expression pattern for a valid normalized formula
     *
     * \par Normalized formula pattern
     * The pattern is defined in the *FORMULA_RE* macro and matches something like
     * \code
     [optional-formula-name]{list,of,sources} function(par1,par2,..,parN) { // javascript function body }
     * \endcode
     * \note
     * without the starting <em>formula://</em> domain declaration.
     *
     */
    virtual QString normalizedFormulaPattern() const = 0;

    /**
     * @brief isNormalized returns true if the expression is valid and normalized
     * @param expr the expression to evaluate
     * @return true if expr is valid and normalized
     */
    virtual bool isNormalized(const QString& expr) const = 0;

    /**
     * @brief isValid returns true if expr is valid
     * @param expr the expression to evaluate
     * @return true if valid, false otherwise
     */
    virtual bool isValid(const QString& expr) const = 0;
};

/**
 * @brief The CuFormulaPluginI defines an *interface* for a plugin that evaluates values
 *        obtained by *cumbia* sources and combined into a formula.
 *
 * \par Example usage
 * \code
 *
    #include <cupluginloader.h>
    #include <QPluginLoader>

    CuPluginLoader plulo;
    QString plupath = plulo.getPluginAbsoluteFilePath(CUMBIA_QTCONTROLS_PLUGIN_DIR, "cuformula-plugin.so");
    QPluginLoader pluginLoader(plupath);
    QObject *plugin = pluginLoader.instance();
    if (plugin){
        CuFormulaPluginI *fplu = qobject_cast<CuFormulaPluginI *>(plugin);
        if(fplu) {  // manage errors otherwise
            fplu->initialize(cu_pool, m_ctrl_factory_pool);
        }
    }  // manage errors otherwise
 * \endcode
 *
 */
class CuFormulaPluginI {
  public:

    virtual ~CuFormulaPluginI() {}

    /**
     * @brief getName returns the name of the plugin
     * @return the name of the plugin as string
     *
     * \par Note
     * CuFormulaPlugin will return *CuFormulaPlugin* as the plugin name
     */
    virtual QString getName() const = 0;

    /**
     * @brief getCumbia The formula plugin allocates a *cumbia formula* implementation, mainly provided
     *        for *API* compatibility. It is deleted by the plugin when the latter is destroyed.
     *
     * @return a pointer to the Cumbia extension managed by the plugin
     */
    virtual Cumbia* getCumbia() const = 0;

    /**
     * @brief getFormulaReaderFactory returns a new instance of an object implementing CuControlsReaderFactoryI
     * @param cu_poo a pointer to a CumbiaPool object
     * @param fpool a const reference to a CuControlsFactoryPool
     * @return a pointer to an object that implements CuControlsReaderFactoryI interface or null if initialize
     *         has not been called yet
     *
     * \par Note
     * The returned value will be null before calling initialize
     *
     */
    virtual CuControlsReaderFactoryI* getFormulaReaderFactory() const = 0;

    /**
     * @brief initialize Initialize the plugin providing a pointer to a valid CumbiaPool and a const reference to
     *        a CuControlsFactoryPool.
     *
     * This method initializes the plugin and makes it ready for usage
     * @param cu_poo a valid pointer to a previously allocated instance of CumbiaPool
     * @param fpool a reference to a CuControlsFactoryPool.
     *
     * \par Note
     * After the method call, srcPatterns will be added to the CuControlsFactoryPool and the CumbiaPool
     * input arguments.
     */
    virtual void initialize(CumbiaPool *cu_poo, CuControlsFactoryPool &fpool) = 0;

    /**
     * @brief message returns the error message
     * @return if a method of the class encounters an error, it will provide a message as well
     */
    virtual std::string message() const = 0;

    /**
     * @brief error returns true if an error happened somewhere when calling one of the
     *        methods of this class
     *
     * @return true if an error happened, false otherwise.
     *
     * \par Note
     * Each method may reset the error condition before performing its work.
     * You should check for errors after each method call.
     */
    virtual bool error() const = 0;

    /**
     * @brief addSrcPattern add a string to the source patterns
     * @param pat a new regular expression pattern describing the form of a valid source
     */
    virtual void addSrcPattern(const std::string& pat) = 0;

    /**
     * @brief srcPatterns returns a list of regular expression patterns for the sources admitted by the
     *         engines.
     *
     * @return a list of regular expression patterns for the sources admitted by the
     *         engines.
     */
    virtual std::vector<std::string> srcPatterns() const = 0;

    /**
     * @brief getFormulaParserInstance returns a new instance of the CuFormulaParserI in use
     * @return a new CuFormulaParserI object
     *
     * \note
     * It is responsibility of the caller to free the object later
     */
    virtual CuFormulaParserI *getFormulaParserInstance() const = 0;

};

#define CuFormulaPluginI_iid "eu.elettra.qutils.CuFormulaPluginI"

Q_DECLARE_INTERFACE(CuFormulaPluginI, CuFormulaPluginI_iid)

#endif // CUFORMULASPLUGIN_INTERFACE_H
