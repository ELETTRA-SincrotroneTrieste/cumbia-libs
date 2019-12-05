#ifndef Qumbiareader_H
#define Qumbiareader_H

#include <QObject>
#include <qulogimpl.h>
#include <cucontrolsfactorypool.h>
#include <QMap>

#include "rconfig.h"

class CuData;
class CumbiaPool;
class RConfig;

/*!
 * \mainpage cumbia read command line client
 *
 * \par Introduction
 * The *qumbia-reader* (*cumbia read*) application is a command line tool to read *sources* from different engines.
 * EPICS, Tango and *random* modules are supported.
 *
 * \par **Features**
 *
 * \par Source name *auto completion* (Tango only, for the time being)
 * Install the <a href="https://github.com/ELETTRA-SincrotroneTrieste/qumbia-tango-findsrc-plugin">qumbia-tango-findsrc-plugin</a>
 * from *github* and the included *qumbia-tango-find-src* tool (instructions in the project page) to obtain
 * the Tango source **bash auto completion**.
 *
 * \par Formula support
 * Formulas can be used through the <a href="../../cuformula/html/index.html">formula plugin</a>. See the
 * plugin documentation and the example section below for more details.
 *
 * \par Library internals awareness
 * Increase the output detail level to see how threads are grouped, understand *how* they are grouped
 * by means of the <a href="../../cumbia-random/html">cumbia random</a> test features.
 * Set a bound on the number of threads used by *timers* for polled sources to verify the impact on
 * performance.
 *
 * \par *Property mode*: get the configuration of the sources
 * Source configuration stored somewhere by the system can be obtained (if this is applies to the engine)
 * **Tango** *device, attribute and class* properties can be fetched from the Tango database as well
 * (requires Tango modules).
 *
 * \par Tune application output
 * The command line output can be tuned in terms of
 * - detail level (low, medim, high and debug)
 * - *format* of numbers (*printf style*)
 * - length of displayed data for arrays
 *
 * \par Usage
 *
 * *cumbia* installation provides two shortcuts to launch the *qumbia-reader* application:
 * - *cumbia read*: read once or *n* times the specified source(s)
 * - *cumbia monitor*: indefinitely monitor the specified source(s) until a key is pressed
 * The latter, executes qumbia-reader --monitor
 *
 * Started without options (equal to *--help*), the tool prints a summary of its functions
 *
 * The list of sources ensue. The implementation supports Tango, EPICS and *cumbia-random* sources.
 * For the latter, the random:// prefix is required. It is recommended to read the
 * <a href="../../cumbia-random/html">the cumbia random module</a> documentation to learn about the
 * required syntax for the sources.
 *
 * \par Read
 * Simply call *cumbia read* followed by the list of sources. Sources from different engines can be mixed.
 * By default, one reading is performed, unless --x is specified:
 *
 * \code cumbia read test/device/1/double_scalar giacomo:ai1 random://rnd/1/0/10 \endcode
 *
 * Output:
 *
 *
 * \code cumbia read inj/diagnostics/rtbpm_inj.01//GetHorPos[0,1000] --truncate=8 --3 --format="%.4f" \endcode
 *
 * Output of last command:
 *
 * \code
 * inj/diagnostics/rtbpm_inj.01->GetHorPos(0,1000): [12:12:23+0.716406] [double,1000] { -0.0228,-0.0091,-0.0158,-0.0180,-0.0232,-0.0160,-0.0124, ..., -0.0178 }
 * inj/diagnostics/rtbpm_inj.01->GetHorPos(0,1000): [12:12:24+0.883161] [double,1000] { -0.0190,-0.0145,-0.0172,-0.0174,-0.0173,-0.0198,-0.0131, ..., -0.0157 }
 * inj/diagnostics/rtbpm_inj.01->GetHorPos(0,1000): [12:12:25+0.885003] [double,1000] { -0.0132,-0.0187,-0.0174,-0.0091,-0.0112,-0.0142,-0.0135, ..., -0.0129 }
 * \endcode
 *
 * \note
 * In order to be auto completed and easily understood by the console without escape characters, **Tango
 * commands** can be written in the form:
 *
 * \code tango/device/name//command_name[arg1,arg2] \endcode
 *
 * instead of the classical syntax \code tango/device/name->command_name(arg1,arg2) \endcode
 * that would require inverted commas:
 *
 * \code cumbia read "inj/diagnostics/rtbpm_inj.01->GetHorPos(0,1000)" \endcode
 *
 * Both forms are accepted, but auto completion adopts the first syntax.
 *
 * \par Monitor
 * Simply replace *cumbia read* with *cumbia monitor* to start monitoring one or more sources. Interrupt
 * the process pressing any key.
 *
 * \par Read (or monitor) and show more details
 *
 *
 */
class QumbiaReader : public QObject
{
    Q_OBJECT

public:
    enum Verbosity { Low, Medium, High, Debug };

    explicit QumbiaReader(CumbiaPool *cu_p, QWidget *parent = 0);
    ~QumbiaReader();

    QString makeTimestamp(const double d) const;
    bool usage_only() const;

public slots:
    void onNewDouble(const QString &src, double ts, double val, const CuData& da);
    void onNewFloat(const QString &src, double ts, float val, const CuData& da);
    void onNewShort(const QString &src, double ts, short val, const CuData& da);
    void onNewBool(const QString &src, double ts, bool val, const CuData& da);
    void onNewUShort(const QString &src, double ts, unsigned short val, const CuData& da);
    void onNewLong(const QString &src, double ts, long val, const CuData& da);
    void onNewULong(const QString &src, double ts, unsigned long val, const CuData& da);
    void onNewString(const QString &src, double ts, const QString& val, const CuData& da);
    void onStringConversion(const QString& src, const QString& fromType, double timestamp_us, const QString& v, const CuData& da);

    void onNewDoubleVector(const QString &src, double ts, const QVector<double> &v, const CuData& da);
    void onNewFloatVector(const QString &src, double ts, const QVector<float> &v, const CuData& da);
    void onNewBoolVector(const QString &src, double ts, const QVector<bool> &v, const CuData& da);
    void onNewShortVector(const QString &src, double ts, const QVector<short> &v, const CuData& da);
    void onNewUShortVector(const QString &src, double ts, const QVector<unsigned short> &v, const CuData& da);
    void onNewLongVector(const QString &src, double ts, const QVector<long> &v, const CuData &da);
    void onNewULongVector(const QString &src, double ts, const QVector<unsigned long> &v, const CuData &da);
    void onNewStringList(const QString &src, double ts, const QStringList& val, const CuData& da);
    void onStringListConversion(const QString& src, const QString& fromType, double timestamp_us, const QStringList& v, const CuData& da);

    void onPropertyReady(const QString& src, double ts, const CuData& p);
    void onError(const QString& src, double ts, const QString& msg, const CuData& da);

private slots:
    void onReaderDestroyed(QObject *o);


private:
    CumbiaPool *cu_pool;
    QuLogImpl m_log_impl;
    CuControlsFactoryPool m_ctrl_factory_pool;
    QList <QObject *> m_readers;
    RConfig m_conf;

    void m_print_extra1(const CuData& da);
    void m_print_extra2(const CuData& da);

    void m_checkRefreshCnt(QObject *o);
    void m_createReaders(const QStringList &srcs);

    void m_print_list_props(const CuData& pr);

    void m_print_property(const CuData& pr);

    template <typename T >
    QString m_format(const T& v, const char *fmt) const;

    QMap<QString, int> m_refreshCntMap;
    QMap<int, QStringList> m_props_map;

};

#endif // Cumbiareader_H
