#ifndef QUINPUTOUTPUT_H
#define QUINPUTOUTPUT_H

#include <einputoutputwidget.h>
#include <cucontrolsfactories_i.h>
#include <cucontrolsfactorypool.h>
#include <cudata.h>
#include <cucontexti.h>

class Cumbia;
class CumbiaPool;
class QPushButton;
class CuContext;

class QuInputOutputPrivate;

/** \brief This element is a graphical interface to integrate a reader and a writer into
 *         one single object that saves space in the user interface.
 *
 * \ingroup outputw
 * \ingroup inputw
 *
 * The source linked to this object is read and displayed on an outputWidget and can be
 * written by an inputWidget. Two contexts are available for this element, an input and
 * an output context. The first is the output widget's context, the second is the input
 * widget's. getOutputContext and getInputContext return a reference to them respectively.
 *
 * By default a QuLabel is used as output widget.
 * A QuButton is used to apply the value edited in the input widget. It can be retrieved with
 * getApplyButton.
 * The input widget is chosen automatically. setWriterType can be manually used to set it manually.
 *
 * QuInputOutput is composed by output and input widgets that are already part of the cumbia
 * widget. It does not use a CuContext of its own.
 *
 * Output widget's newData(const CuData&) signal can be connected to a consistent receiver's slot if
 * this needs to be notified:
 *
 * \code
 * connect(qu_io->outputWidget(), SIGNAL(newData(const CuData&)), receiver, SLOT(onNewData(const CuData &)));
 * \endcode
 *
 */
class QuInputOutput : public EInputOutputWidget, public CuContextI
{
    Q_OBJECT
    Q_PROPERTY(QString source READ source WRITE setSource DESIGNABLE true)

public:
    /** \brief this enum describes the type of available writers
     *
     */
    enum WriterType { None, LineEdit, Numeric, SpinBox, DoubleSpinBox, ComboBox };

    QuInputOutput(QWidget *_parent,
                  Cumbia *cumbia,
                  const CuControlsReaderFactoryI &r_fac,
                  const CuControlsWriterFactoryI &w_fac);

    QuInputOutput(QWidget *parent, CumbiaPool* cu_poo, const CuControlsFactoryPool& f_poo);

    ~QuInputOutput();

    void setWriterType(WriterType t);

    WriterType writerType() const;

    QPushButton *getApplyButton();

    QString source() const;

    CuContext *getContext() const;

    CuContext *getOutputContext() const;

    CuContext *getInputContext() const;

    void setObjectName(const QString& name);

public slots:
    virtual void onNewData(const CuData& d);

    void setSource(const QString& s);

    void unsetSource();

private:
    QuInputOutputPrivate *d;

    void m_init();

    void m_configure(const CuData& da);
};

#endif // QUINPUTOUTPUT_H
