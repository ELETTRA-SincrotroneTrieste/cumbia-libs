#ifndef QUBUTTON_H
#define QUBUTTON_H

#include <QPushButton>
#include <cudatalistener.h>

class QuButtonPrivate;
class Cumbia;
class CuControlsWriterFactoryI;

class QuButton : public QPushButton, public CuDataListener
{
    Q_OBJECT
    Q_PROPERTY(QString targets READ targets WRITE setTargets DESIGNABLE true)
public:
    QuButton(QWidget *parent, Cumbia *cumbia, const CuControlsWriterFactoryI &w_fac);

    virtual ~QuButton();

    QString targets() const;

public slots:
    virtual void execute();

    void setTargets(const QString& targets);


private:
    QuButtonPrivate *d;

    // CuTangoListener interface
public:
    void onUpdate(const CuData &d);
};

#endif // QUPUSHBUTTON_H
