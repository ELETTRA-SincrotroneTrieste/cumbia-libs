#ifndef QUTANGOCONTROLSPLUGINS_H
#define QUTANGOCONTROLSPLUGINS_H

#include <QtGlobal> /* for QT_VERSION */

#if QT_VERSION < 0x050500
    #include <QDesignerCustomWidgetInterface>
#else
    #include <QtUiPlugin/QDesignerCustomWidgetInterface>

#endif

#include <QDesignerTaskMenuExtension>
#include <QExtensionFactory>

#include "editsourcedialog.h"
#include "edittargetdialog.h"
#include "pointeditor.h"

class CumbiaTango;
class EditSourceDialog;

/** \brief intercepts DropEvents and pops up a Dialog to configure the source
  *        or the target of the object.
  *
  * Just drag and drop an attribute or a command from jive or from any other QTango panel
  * into any of the QTango widgets exported by the Qt designer and quickly configure its
  * source or target.
  */
class DropEventFilter : public QObject
{
    Q_OBJECT
public:
    DropEventFilter(QObject *parent);

 protected:
    bool eventFilter(QObject *obj, QEvent *event);

};

class CustomWidgetInterface: public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    CustomWidgetInterface(QObject *parent, CumbiaTango *cumbia_t);

    virtual bool isContainer() const { return false; }
    virtual bool isInitialized() const { return d_isInitialized; }
    virtual QIcon icon() const { return d_icon; }
    virtual QString codeTemplate() const { return d_codeTemplate; }
    virtual QString domXml() const { return d_domXml; }
    virtual QString group() const { return "QumbiaTangoControls"; }
    virtual QString includeFile() const { return d_include; }
    virtual QString name() const { return d_name; }
    virtual QString toolTip() const { return d_toolTip; }
    virtual QString whatsThis() const { return d_whatsThis; }
    virtual void initialize(QDesignerFormEditorInterface *);

protected:
    QString d_name;
    QString d_include;
    QString d_toolTip;
    QString d_whatsThis;
    QString d_domXml;
    QString d_codeTemplate;
    QIcon d_icon;
    CumbiaTango *cumbia_tango;

private:
    bool d_isInitialized;
};

class CustomWidgetCollectionInterface:
        public QObject,
        public QDesignerCustomWidgetCollectionInterface
{
    Q_OBJECT

#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QDesignerCustomWidgetCollectionInterface")
#endif

    Q_INTERFACES(QDesignerCustomWidgetCollectionInterface)

public:
    CustomWidgetCollectionInterface(QObject *parent = NULL);

    virtual QList<QDesignerCustomWidgetInterface*> customWidgets() const;

private:
    QList<QDesignerCustomWidgetInterface*> d_plugins;

    CumbiaTango *cumbia_tango;
};

class QuLabelInterface : public CustomWidgetInterface
{
   Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

  public:
    QuLabelInterface(QObject* parent, CumbiaTango *cumbia_t);
    virtual QWidget* createWidget(QWidget* parent);
};

class QuButtonInterface : public CustomWidgetInterface
{
   Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

  public:
    QuButtonInterface(QObject* parent, CumbiaTango *cumbia_t);
    virtual QWidget* createWidget(QWidget* parent);
};

class QuLedInterface : public CustomWidgetInterface
{
   Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

  public:
    QuLedInterface(QObject* parent, CumbiaTango *cumbia_t);
    virtual QWidget* createWidget(QWidget* parent);
};

class QuLineEditInterface : public CustomWidgetInterface
{
   Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

  public:
    QuLineEditInterface(QObject* parent, CumbiaTango *cumbia_t);
    virtual QWidget* createWidget(QWidget* parent);
};

class QuTableInterface : public CustomWidgetInterface
{
   Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

  public:
    QuTableInterface(QObject* parent, CumbiaTango *cumbia_t);
    virtual QWidget* createWidget(QWidget* parent);
};

class TaskMenuFactory: public QExtensionFactory
{
Q_OBJECT
public:
    TaskMenuFactory(QExtensionManager *parent = 0);

protected:
    QObject *createExtension(QObject *object, const QString &iid, QObject *parent) const;
};

class TaskMenuExtension: public QObject, public QDesignerTaskMenuExtension
{
Q_OBJECT
Q_INTERFACES(QDesignerTaskMenuExtension)

public:
    TaskMenuExtension(QWidget *widget, QObject *parent);

    QAction *preferredEditAction() const;
    QList<QAction *> taskActions() const;

private slots:
    void editTango();
    void editAttributes();

protected:
    QWidget *d_widget;
    QAction *d_editTangoAction;
    QAction *d_editAction;
    void setupSourceTargetDialog(QWidget *s);

    EditSourceDialog *editSourceDialog;
};

#if QT_VERSION < 0x050000
    Q_EXPORT_PLUGIN2(QtControls, CustomWidgetCollectionInterface)
#endif

#endif // QUTANGOCONTROLSPLUGINS_H
