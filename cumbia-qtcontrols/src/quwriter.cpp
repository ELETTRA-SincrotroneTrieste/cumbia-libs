#include "quwriter.h"
#include "qumbiaizerprivate.h" /* for TYPELEN */
#include <cumacros.h>
#include <cudata.h>
#include <cumbia.h>
#include <cuserviceprovider.h>

#include "cucontrolswriter_abs.h"
#include "cucontrolsfactories_i.h"
#include "cucontrolsutils.h"
#include "cumbiapool.h"
#include "cucontext.h"
#include "qulogimpl.h"
#include "cuctx_swap.h"

#include <QTimer>

class Cumbia;
class CumbiaPool;
class CuControlsFactoryPool;
class CuControlsWriterFactoryI;

class QuWriterPrivate
{
public:
    CuContext *context;
    bool auto_configure;
    bool write_ok;
    bool configured;
    CuLog *log;
};

QuWriter::QuWriter(QObject *parent, Cumbia *cumbia, const CuControlsWriterFactoryI &w_fac) :
    Qumbiaizer(parent)
{
    m_init();
    d->context = new CuContext(cumbia, w_fac);
}

QuWriter::QuWriter(QObject *parent, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool) :
    Qumbiaizer(parent)
{
    m_init();
    d->context = new CuContext(cumbia_pool, fpool);
}

QuWriter::~QuWriter()
{
    delete d->context;
    delete d;
}

void QuWriter::m_init()
{
    d = new QuWriterPrivate;
    d->auto_configure = true;
    d->log = NULL;
    d->write_ok = d->configured = false;
}

void QuWriter::attach(QObject *executor,  const char *signal,
                      const char *setPointSlot, Qt::ConnectionType connType)
{
    char in_type[TYPELEN];
    Qumbiaizer::attach(executor, signal, setPointSlot, connType);
    QString method = QString(signal);
    /* remove the signal code at the beginning of the method */
    extractCode(method);
    if(inTypeOfMethod(qstoc(method), executor, in_type))
    {
        bool connectOk = false;
        if(strlen(in_type) == 0)
            connectOk = connect(executor, signal, this, SLOT(execute()));
        else  if(strcmp(in_type, "QStringList") == 0)
            connectOk = connect(executor, signal, this, SLOT(execute(const QStringList&)));
        else  if(strcmp(in_type, "QVector<double>") == 0)
            connectOk = connect(executor, signal, this, SLOT(execute(const QVector<double>&)));
        else  if(strcmp(in_type, "QVector<int>") == 0)
            connectOk = connect(executor, signal, this, SLOT(execute(const QVector<int>&)));
        else  if(strcmp(in_type, "QVector<bool>") == 0)
            connectOk = connect(executor, signal, this, SLOT(execute(const QVector<bool>&)));
        else if(!strcmp(in_type, "int"))
            connectOk = connect(executor, signal, this, SLOT(execute(int)));
        else if(!strcmp(in_type, "bool"))
            connectOk = connect(executor, signal, this, SLOT(execute(bool)));
        else if(!strcmp(in_type, "double"))
            connectOk = connect(executor, signal, this, SLOT(execute(double)));
        else if(!strcmp(in_type, "short"))
            connectOk = connect(executor, signal, this, SLOT(execute(short)));
        else if(!strcmp(in_type, "unsigned int"))
            connectOk = connect(executor, signal, this, SLOT(execute(unsigned int)));
        else if(strstr(in_type, "String") != NULL)
            connectOk =  connect(executor, signal, this, SLOT(execute(const QString&)));


        else
        {
            perr("QuWriter::attach: signal \"%s\" of object \"%s\" is not compatible with supported types",
                 qstoc(method), qstoc(executor->objectName()));
            perr("                  supported types: \"int\", \"unsigned int\", \"short\", \"double\", \"const QString&\"");
        }

        if(!connectOk)
        {
            perr("QuWriter::attach: failed to connect signal \"%s\" of object \"%s\" to the ",
                 qstoc(method), qstoc(executor->objectName()));
            perr("                 corresponding execute() slot");
        }
    }
    else
        printf("\e[1;31mQuWriter::attach: inTypeOfMethod failed, in_type \"%s\" method \"%s\" obj \"%s\"\e[0m\n",
               in_type, qstoc(method), qstoc(executor->objectName()));

}

void QuWriter::postExecute() {
    if(quizer_ptr->autoDestroy) {
        QTimer::singleShot(2000, this, SLOT(deleteLater()));
    }
}

void QuWriter::setTarget(const QString &target)
{
    // only one writer at a time: use replace_writer from CuContext
    CuControlsWriterA* w = d->context->replace_writer(target.toStdString(), this);
    if(w)
        w->setTarget(target); // setTargets must be called on a valid writer
}

QString QuWriter::target() const
{
    CuControlsWriterA *w = d->context->getWriter();
    if(w != NULL)
        return w->target(); // if the writer is configured
    return ""; // empty string otherwise
}

bool QuWriter::ctxSwap(CumbiaPool *cu_p, const CuControlsFactoryPool &fpool) {
    CuCtxSwap csw;
    d->context = csw.replace(this, d->context, cu_p, fpool);
    return csw.ok();
}

void QuWriter::execute(int i) {
    CuControlsWriterA *w = d->context->getWriter();
    if(!w || !d->configured)
        quizer_ptr->setExecuteOnConnection(true, CuVariant(i));
    else {
        w->setArgs(CuVariant(i));
        w->execute();
        postExecute();
    }
}

void QuWriter::execute(short s)
{
    CuControlsWriterA *w = d->context->getWriter();
    if(!w || !d->configured)
        quizer_ptr->setExecuteOnConnection(true, CuVariant(s));
    else
    {
        w->setArgs(CuVariant(s));
        w->execute();
        postExecute();
    }
}

void QuWriter::execute(const QString& s)
{
    CuControlsWriterA *w = d->context->getWriter();
    if(!w || !d->configured)
        quizer_ptr->setExecuteOnConnection(true, CuVariant(s.toStdString()));
    else
    {
        w->setArgs(CuVariant(s.toStdString()));
        w->execute();
        postExecute();
    }
}

void QuWriter::execute(double dou)
{
    CuControlsWriterA *w = d->context->getWriter();
    if(!w || !d->configured)
        quizer_ptr->setExecuteOnConnection(true, CuVariant(dou));
    else
    {
        w->setArgs(CuVariant(dou));
        w->execute();
        postExecute();
    }
}

void QuWriter::execute(unsigned int i)
{
    CuControlsWriterA *w = d->context->getWriter();
    if(!w || !d->configured)
        quizer_ptr->setExecuteOnConnection(true, CuVariant(i));
    else
    {
        w->setArgs(CuVariant(i));
        w->execute();
        postExecute();
    }
}

void QuWriter::execute(bool bo)
{
    CuControlsWriterA *w = d->context->getWriter();
    if(!w || !d->configured)
        quizer_ptr->setExecuteOnConnection(true, CuVariant(bo));
    else
    {
        w->setArgs(CuVariant(bo));
        w->execute();
        postExecute();
    }
}

/** \brief empty argument version. Takes the input arguments to write from the targets
 *
 * targets are analysed and an input argument is extracted, if possible. Otherwise the
 * writing is performed with an empty value. For example, if targets are
 *
 * <em>p/power_supply/super_ps/Current(&doubleSpinboxCurrent)</em>
 *
 * the value of the current is taken by the spinbox with name doubleSpinboxCurrent
 */
void QuWriter::execute()
{
    CuControlsUtils cu;
    CuVariant args = cu.getArgs(target(), this);
    CuControlsWriterA *w = d->context->getWriter();
    if(!w || !d->configured)
        quizer_ptr->setExecuteOnConnection(true, args);
    else
    {
        w->setArgs(args);
        w->execute();
        postExecute();
    }
}

void QuWriter::execute(const QStringList& sl)
{
    std::vector<std::string> vs;
    foreach(QString s, sl)
        vs.push_back(s.toStdString());
    CuControlsWriterA *w = d->context->getWriter();
    CuVariant v(vs);
    if(!w || !d->configured)
        quizer_ptr->setExecuteOnConnection(true, v);
    else
    {
        w->setArgs(v);
        w->execute();
        postExecute();
    }
}

void QuWriter::execute(const QVector<int>& iv)
{
    std::vector <int> v(iv.begin(), iv.end());
    CuControlsWriterA *w = d->context->getWriter();
    CuVariant vv(v);
    if(!w || !d->configured)
        quizer_ptr->setExecuteOnConnection(true, vv);
    else
    {
        w->setArgs(vv);
        w->execute();
        postExecute();
    }
}

void QuWriter::execute(const QVector<double>& dv)
{
    std::vector <double> v(dv.begin(), dv.end());
    CuControlsWriterA *w = d->context->getWriter();
    CuVariant vv(v);
    if(!w || !d->configured)
        quizer_ptr->setExecuteOnConnection(true, vv);
    else
    {
        w->setArgs(vv);
        w->execute();
        postExecute();
    }
}

void QuWriter::execute(const QVector<bool>& bv) {
    std::vector <bool> v(bv.begin(), bv.end());
    CuControlsWriterA *w = d->context->getWriter();
    CuVariant vv(v);
    if(!w || !d->configured)
        quizer_ptr->setExecuteOnConnection(true, vv);
    else
    {
        w->setArgs(vv);
        w->execute();
        postExecute();
    }
}

void QuWriter::onUpdate(const CuData &data)
{
    d->configured = true;
    quizer_ptr->error = data["err"].toBool();
    quizer_ptr->message = QString::fromStdString(data["msg"].toString());
    if(data["type"].toString() == "property" && d->auto_configure) {
        Qumbiaizer::configure(data); // emits configured and connectionOk
        CuControlsWriterA *w = d->context->getWriter();
        if(quizer_ptr->executeOnConnection() && w) {
            CuVariant arg = quizer_ptr->executeArgument();
            if(!arg.isNull())
                w->setArgs(arg);
            w->execute();
            postExecute();
        }
    }

    emit newData(data);
}


