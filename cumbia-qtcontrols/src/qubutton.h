#ifndef QUBUTTON_H
#define QUBUTTON_H

#include <QPushButton>
#include <cudatalistener.h>
#include <cucontexti.h>

class QuButtonPrivate;
class Cumbia;
class CumbiaPool;
class CuControlsFactoryPool;
class CuControlsWriterFactoryI;

/** \brief A push button to write values or send commands to a target.
 *
 * \ingroup inputw
 *
 * Derives from QPushButton and acts as a writer when clicked.
 * Set up the link with setTarget.
 * When clicked, the execute method is called, the (optional) input arguments
 * are evaluated and the write is performed on the target.
 *
 * See also \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 *
 */
class QuButton : public QPushButton, public CuDataListener, public CuContextI
{
    Q_OBJECT
    Q_PROPERTY(QString target READ target WRITE setTarget DESIGNABLE true)
public:
    QuButton(QWidget *parent, Cumbia *cumbia, const CuControlsWriterFactoryI &w_fac, const QString& text = "Apply");
    QuButton(QWidget *w, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool, const QString& text = "Apply");

    virtual ~QuButton();

    QString target() const;

    // CuTangoListener interface
    void onUpdate(const CuData &d);

    // CuContextI interface
    CuContext *getContext() const;

public slots:
    virtual void execute();

    void setTarget(const QString& target);

signals:
    void linkStatsRequest(QWidget *myself, CuContextI *myself_as_cwi);
    void newData(const CuData& da);

protected:
    void paintEvent(QPaintEvent *pe);

    void contextMenuEvent(QContextMenuEvent *e);

private:
    QuButtonPrivate *d;
    void m_init(const QString& text);

private slots:
    void onAnimationValueChanged(const QVariant& v);

};

#endif // QUPUSHBUTTON_H
