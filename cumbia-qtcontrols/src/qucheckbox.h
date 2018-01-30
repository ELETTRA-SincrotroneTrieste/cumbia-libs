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

/** \brief A check box that reads, displays and writes a boolean value.
 *
 * Derives from QCheckBox, acts as a reader to display a boolean value and as
 * a writer when it is checked.
 * When checked, the current value read from the source is inverted and written on the same
 * source.
 *
 * setSource creates the link to the specified source.
 * The textFromLabel property, if true, reads the "label" value from the CuData
 * delivered at configuration time and sets
 * the text on the check box accordingly (Tango attributes have a *label*
 * attribute property that can be used, see <a href="../../cumbia-tango/md_lib_cudata_for_tango.html">CuData bundle description for data exchange with the Tango world.</a>).
 * Otherwise, provide a text as you would do for a Qt QCheckBox.
 *
 */
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
