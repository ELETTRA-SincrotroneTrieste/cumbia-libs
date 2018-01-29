#ifndef QuWriter_H
#define QuWriter_H

#include <cudatalistener.h>
#include "qumbiaizer.h"

#include <cumacros.h>   // pinfo pwarn perr qstoc
#include <cudata.h>     // exchanged data class definition
#include <cumbia.h>     // cumbia

#include "cucontrolswriter_abs.h"
#include "cucontrolsfactories_i.h"
#include "cucontrolsutils.h"
#include "cumbiapool.h"
#include "cucontext.h"
#include "qulogimpl.h"

class QuWriterPrivate;

/** \brief A class that can be used to write values from (almost) any kind of widget
  *        or object.
  *
  */
class QuWriter : public Qumbiaizer, public CuDataListener
{
    Q_OBJECT

public:
    /** \brief the class constructor
      *
      * @param parent the QObject which will be the parent for the QuWriter.
      */
    QuWriter(QObject *parent, Cumbia *cumbia, const CuControlsWriterFactoryI &w_fac);

    QuWriter(QObject *w, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool);

    virtual ~QuWriter();

    /** \brief Signal signal of the executor is connected to the execute method of this class.
      *
      * The available slots are listed below.
      * @param executor the QObject to which we connect to receive an execution signal
      * @param signal the signal name, declared with the SIGNAL macro which we are listening for
      * @param setPointSlot provide a custom slot to initialize the set point value inside the
      *        auto configuration method.
      * @param connType the Qt connection type.
      *
      * \par Example
      * \code
      * QuWriter *dualWriter = new QuWriter(this);
      * dualWriter->attach(ui.writeDial, SIGNAL(sliderMoved(int)), SLOT(setValue(int)));
      * dualWriter->setTargets("test/device/1/double_scalar");
      *
      * \endcode
      *
      * Every time the slider is moved, it emits sliderMoved(int) signal. The QuWriter <strong>
      * internally connects</strong> sliderMoved(int) signal to the <strong> execute(int)</strong>
      * slot, recognizing the signal input parameter type.
      * In the example above, when auto configuration  of "test/device/1/double_scalar" is available,
      * the current set point of double_scalar is set on the widget via the setValue(int) SLOT.
     */
    virtual void attach(QObject *executor, const char *signal, const char *setPointSlot = NULL,
                        Qt::ConnectionType connType = Qt::AutoConnection);

    /** \brief executes some code after QTangoComProxyWriter::execute is called.
      *
      * Normally it tests whether the user wants the object to be destroyed after execution
      */
    void postExecute();

    QString targets() const;

public slots:

    void setTargets(const QString& targets);

    /** \brief the execute method tailored for integer values
      *
      * The execute method can be reimplemented in subclasses to specialize its behaviour.
      * By default, the signal provided inside attach method is connected to this slot, when
      * the signal has an integer input type.
      * For instance, when the value of a QSpinBox changes, this slot can be invoked.
      */
    virtual void execute(int);

    /** \bref the execute method tailored for short integer values
      *
      * @see void execute(int)
      */
    virtual void execute(short);

    /** \bref the execute method tailored for string values
      *
      * @see void execute(int)
      */
    virtual void execute(const QString&);

    /** \bref the execute method tailored for double values
      *
      * @see void execute(int)
      */
    virtual void execute(double);

    /** \bref the execute method tailored for unsigned integer values
      *
      * @see void execute(int)
      */
    virtual void execute(unsigned int);

    /** \bref the execute method tailored for a void input value
      *
      * @see void execute(int)
      */
    virtual void execute();

    /** \bref the execute method tailored for boolean values
      *
      * @see void execute(int)
      */
    virtual void execute(bool);

    /** \brief The execute method tailored for a list of strings
      *
      *
      */
    virtual void execute(const QStringList&);

    /** \brief The execute method tailored for a vector of integers
      *
      */
    virtual void execute(const QVector<int>&);

    /** \brief The execute method tailored for a vector of double
      *
      *
      */
    virtual void execute(const QVector<double>&);

    /** \brief The execute method tailored for a vector of booleans
      *
      *
      */
    virtual void execute(const QVector<bool>&);


    // CuDataListener interface
    void onUpdate(const CuData &data);

private:
    QuWriterPrivate *d;

    void m_init();


};

#endif // QuWriter_H
