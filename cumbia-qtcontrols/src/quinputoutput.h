#ifndef QUINPUTOUTPUT_H
#define QUINPUTOUTPUT_H

#include <einputoutputwidget.h>
#include <cucontrolsfactories_i.h>
#include <cucontrolsfactorypool.h>
#include <cudata.h>
#include <cucontextwidgeti.h>

class Cumbia;
class CumbiaPool;
class QPushButton;
class CuContext;

class QuInputOutputPrivate;

class QuInputOutput : public EInputOutputWidget, public CuContextWidgetI
{
    Q_OBJECT
    Q_PROPERTY(QString source READ source WRITE setSource DESIGNABLE true)

public:
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
