#ifndef QUCHECKBOX_H
#define QUCHECKBOX_H

#include <QCheckBox>
#include <cucontexti.h>
#include <cudatalistener.h>

class QuCheckboxPrivate;
class Cumbia;
class CumbiaPool;
class CuControlsReaderFactoryI;
class CuControlsWriterFactoryI;
class CuControlsFactoryPool;
class CuData;
class CuContext;

class QuCheckBox : public QCheckBox, public CuDataListener, public CuContextI
{
    Q_OBJECT
    Q_PROPERTY(QString source READ source WRITE setSource DESIGNABLE true)
    Q_PROPERTY(bool textFromLabel READ textFromLabel WRITE setTextFromLabel DESIGNABLE true)
public:
    QuCheckBox(QWidget *parent, Cumbia* cumbia,
               const CuControlsReaderFactoryI& r_fac,
               const CuControlsWriterFactoryI &w_fac);

     QuCheckBox(QWidget *parent, CumbiaPool* cu_poo, const CuControlsFactoryPool& f_poo);

     virtual ~QuCheckBox();

     QString source() const;

     CuContext *getContext() const;

     CuContext *getOutputContext() const;

     CuContext *getInputContext() const;

     bool textFromLabel() const;

     // CuDataListener interface
     void onUpdate(const CuData &d);

public slots:

    void setSource(const QString& s);

    void setTextFromLabel(bool tfl);

signals:
    void newData(const CuData&);

    void linkStatsRequest(QWidget *myself, CuContextI *myself_as_cwi);

protected:
    void contextMenuEvent(QContextMenuEvent* e);

protected slots:
    virtual void checkboxClicked();

private:
    QuCheckboxPrivate *d;

    void m_init();
};

#endif // QUCHECKBOX_H
