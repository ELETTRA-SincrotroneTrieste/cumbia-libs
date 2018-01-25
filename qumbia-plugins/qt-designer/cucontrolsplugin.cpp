#include "cucontrolsplugin.h"

/* Qu widgets includes */
#include "qulabel.h"
#include "quled.h"
#include "qulineedit.h"
#include "qutable.h"
#include "qutrendplot.h"
#include "quspectrumplot.h"
#include "forms/tlabelbooleditor.h"
#include "forms/tledbooleditor.h"
#include "forms/ttablebooleditor.h"
#include "qubutton.h"
#include "quapplynumeric.h"

#ifdef QUMBIA_EPICS_CONTROLS
    #include <cuepics-world.h>
    #include <cumbiaepics.h>
    #include <cuepactionfactories.h>
    #include <cuepcontrolsreader.h>
    #include <cuepcontrolswriter.h>
#endif

#include <cumbiatango.h>
#include <cumacros.h>
#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>
#include <cutcontrolsreader.h>
#include <cutcontrolswriter.h>
#include <cutangoactionfactories.h>
#include <cutango-world.h>
#include <culog.h>
#include <qulogimpl.h>
#include <cuservices.h>
#include <cuserviceprovider.h>
#include <cumbiapool.h>

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDesignerFormWindowInterface>
#include <QDesignerFormWindowCursorInterface>
#include <QDesignerFormEditorInterface>
#include <QExtensionManager>
#include <QMimeData>

#define SOURCE_REGEXP "(([A-Za-z_0-9\\.]*[:]{1}[0-9]+[/])?(([A-Za-z_0-9\\.]+/[A-Za-z_0-9\\.]+/[A-Za-z_0-9\\.]+([/]{1,1}|[->]{2,2})[A-Za-z_0-9\\.]+)|(\\$[1-9]*([/]{1,1}|[->]{2})[A-Za-z_0-9\\.]+)){1}([\\(]{1}[&A-Za-z_0-9\\.,]+[\\)]{1})?[;]?){1,}"


DropEventFilter::DropEventFilter(QObject *parent) : QObject(parent)
{

}

bool DropEventFilter::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::DragEnter)
    {
        QDragEnterEvent *dev = static_cast<QDragEnterEvent *>(event);
        dev->acceptProposedAction();
        return true; /* stop handling event here */
    }
    else if(event->type() == QEvent::Drop)
    {
        QDropEvent *dropEvent = static_cast<QDropEvent *>(event);
        const QMimeData *mimeData = dropEvent->mimeData();
        QString point = mimeData->text();

        if(point.contains(QRegExp(SOURCE_REGEXP)) )
        {
            QDesignerFormWindowInterface *formWindow = 0;
            formWindow = QDesignerFormWindowInterface::findFormWindow(obj);
            if(formWindow)
            {
                formWindow->clearSelection(true);
                formWindow->selectWidget(qobject_cast<QWidget *>(obj));

                PointEditor pointEditor(qobject_cast<QWidget *>(obj), point);

                /* TCheckBox for convenience sets sources and targets */
                if(obj->metaObject()->indexOfProperty("targets") > -1 &&
                        obj->inherits("TCheckBox"))
                {
                    pointEditor.textLabel()->setText("TCheckBox: set <b>source</b> and <b>target</b> point");
                    pointEditor.setWindowTitle("TCheckBox source and target editor");
                }
                else if(obj->metaObject()->indexOfProperty("source") > -1)
                {
                    pointEditor.textLabel()->setText("Set <b>source</b> point on " + obj->objectName());
                    pointEditor.setWindowTitle(QString("%1 source editor").arg(obj->objectName()));
                }
                else if(obj->metaObject()->indexOfProperty("targets") > -1 && !obj->inherits("TCheckBox"))
                {
                    pointEditor.textLabel()->setText("Set <b>target</b> point on " + obj->objectName());
                    pointEditor.setWindowTitle(QString("%1 target editor").arg(obj->objectName()));
                }
                pointEditor.exec();

                if(pointEditor.result() == QDialog::Accepted)
                {
                    if(obj->metaObject()->indexOfProperty("source") > -1)
                    {
                        formWindow->cursor()->setProperty("source", pointEditor.point());
                    }
                    else if(obj->metaObject()->indexOfProperty("targets") > -1)
                    {
                        formWindow->cursor()->setProperty("targets", pointEditor.point());
                    }
                    /* TCheckBox: set targets too! */
                    if(obj->metaObject()->indexOfProperty("targets") > -1 &&
                            obj->inherits("TCheckBox"))
                        formWindow->cursor()->setProperty("targets", pointEditor.point());
                }

            }
            else
                perr("formWindow is null (\"%s\")", qstoc(obj->objectName()));
        }
        else
            perr("point is empty (cannot set \"%s\" on \"%s\")", qstoc(point), qstoc(obj->objectName()));

        return true; /* always eat drop events */
    }
    return false;
}

CuCustomWidgetInterface::CuCustomWidgetInterface(QObject *parent,
                                             CumbiaPool *cumbia_p, const CuControlsFactoryPool &ctrl_factory_p)
    : QObject(parent)
{
    d_isInitialized = false;
    cumbia_pool = cumbia_p;
    ctrl_factory_pool = ctrl_factory_p;
}

CuCustomWidgetInterface::~CuCustomWidgetInterface()
{

}

void CuCustomWidgetInterface::initialize(QDesignerFormEditorInterface *formEditor)
{
    if (d_isInitialized)
        return;

    QExtensionManager *manager = formEditor->extensionManager();
    if (manager)
        manager->registerExtensions(new TaskMenuFactory(manager), Q_TYPEID(QDesignerTaskMenuExtension));
    d_isInitialized = true;
}


CuCustomWidgetCollectionInterface::CuCustomWidgetCollectionInterface(QObject *parent): QObject(parent)
{
    printf("\e[1;32mo\e[0m CuCustomWidgetCollectionInterface %p\n", this);
    cumbia_pool = new CumbiaPool();
    printf("\e[1;32m+-o\e[0m cumbia_pool %p created\n", cumbia_pool);

#ifdef QUMBIA_EPICS_CONTROLS
    CumbiaEpics* cuep = new CumbiaEpics(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
    printf("\e[1;32m+-o\e[0m cumbia_epics %p created\n", cuep);
    cumbia_pool->registerCumbiaImpl("epics", cuep);
    m_ctrl_factory_pool.registerImpl("epics", CuEpReaderFactory());
    m_ctrl_factory_pool.registerImpl("epics", CuEpWriterFactory());
    CuEpicsWorld ew;
    m_ctrl_factory_pool.setSrcPatterns("epics", ew.srcPatterns());
    cumbia_pool->setSrcPatterns("epics", ew.srcPatterns());
    CuServiceProvider *cuepsp = cuep->getServiceProvider();
    cuepsp->registerService(CuServices::Log, new CuLog(new QuLogImpl()));
#endif

    CumbiaTango* cuta = new CumbiaTango(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
    printf("\e[1;32m+-o\e[0m cumbia_tango %p created\n", cuta);

    cumbia_pool->registerCumbiaImpl("tango", cuta);
    m_ctrl_factory_pool.registerImpl("tango", CuTWriterFactory());
    m_ctrl_factory_pool.registerImpl("tango", CuTReaderFactory());
    printf("\e[1;32m+-o\e[0m registered \"tango\" and \"epics\" implementations in the cumbia_pool %p\n", cumbia_pool);


    CuTangoWorld tw;
    m_ctrl_factory_pool.setSrcPatterns("tango", tw.srcPatterns());
    cumbia_pool->setSrcPatterns("tango", tw.srcPatterns());

    CuServiceProvider* cutangosp = cuta->getServiceProvider();
    cutangosp->registerService(CuServices::Log, new CuLog(new QuLogImpl()));

    d_plugins.append(new QuLabelInterface(this, cumbia_pool, m_ctrl_factory_pool));
    d_plugins.append(new QuLedInterface(this, cumbia_pool, m_ctrl_factory_pool));
    d_plugins.append(new QuLineEditInterface(this, cumbia_pool, m_ctrl_factory_pool));
    d_plugins.append(new QuTableInterface(this, cumbia_pool, m_ctrl_factory_pool));
    d_plugins.append(new QuTrendPlotInterface(this, cumbia_pool, m_ctrl_factory_pool));
    d_plugins.append(new QuSpectrumPlotInterface(this, cumbia_pool, m_ctrl_factory_pool));
    d_plugins.append(new QuButtonInterface(this, cumbia_pool, m_ctrl_factory_pool));
    d_plugins.append(new QuApplyNumericInterface(this, cumbia_pool, m_ctrl_factory_pool));
}

CuCustomWidgetCollectionInterface::~CuCustomWidgetCollectionInterface()
{
    printf("\e[1;31mo\e[0m ~CuCustomWidgetCollectionInterface %p\n", this);
    Cumbia* c = cumbia_pool->get("tango");
    if(c)
    {
        printf("\e[1;31m+--o\e[0m ~cumbia_tango %p\n", c);
        cumbia_pool->unregisterCumbiaImpl("tango");
        delete c;
    }

#ifdef CUMBIA_EPICS
    c = cumbia_pool->get("epics");
    if(c)
    {
        printf("\e[1;31m+--o\e[0m ~cumbia_epics %p\n", c);
        cumbia_pool->unregisterCumbiaImpl("epics");
        delete c;
    }
#endif

    if(cumbia_pool)
    {
        printf("\e[1;31m+--o\e[0m ~cumbia_pool %p\n", cumbia_pool);
        delete cumbia_pool;
    }

}

QList<QDesignerCustomWidgetInterface*> CuCustomWidgetCollectionInterface::customWidgets(void) const
{
    return d_plugins;
}

/* TaskMenuFactory */

TaskMenuFactory::TaskMenuFactory(QExtensionManager *parent): QExtensionFactory(parent)
{
}

QObject *TaskMenuFactory::createExtension(QObject *object, const QString &iid, QObject *parent) const
{
    if (iid != Q_TYPEID(QDesignerTaskMenuExtension))
        return 0;

    if (qobject_cast<QuLabel*>(object) || qobject_cast<QuLed *>(object)
            || qobject_cast<QuButton *>(object) || qobject_cast<QuApplyNumeric *>(object) || qobject_cast<QuTable *>(object)
             || qobject_cast<QuTrendPlot *>(object)|| qobject_cast<QuSpectrumPlot *>(object) )
        return new TaskMenuExtension((QWidget*)object, parent);

    return QExtensionFactory::createExtension(object, iid, parent);
}
/* */

/* TaskMenuExtension */
TaskMenuExtension::TaskMenuExtension(QWidget *widget, QObject *parent): QObject(parent), d_widget(widget), d_editConnectionAction(0), d_editAction(0), editSourceDialog(0)
{
    d_editConnectionAction = new QAction(tr("Edit Connection..."), this);
    d_editAction = new QAction(tr("Edit Properties..."), this);
    connect(d_editConnectionAction, SIGNAL(triggered()), this, SLOT(editConnection()));
    connect(d_editAction, SIGNAL(triggered()), this, SLOT(editAttributes()));
}

QList<QAction *> TaskMenuExtension::taskActions() const
{
    QList<QAction *> list;
    QString cname(d_widget->metaObject()->className());
    /* 1. edit connection action */
    if (cname == "QuLabel" || cname == "QuLed" || cname == "QuLineEdit"
        || cname == "QuButton" || cname == "QuTable" || cname == "QuTrendPlot"
            || cname == "QuSpectrumPlot" || cname == "QuApplyNumeric")
        list.append(d_editConnectionAction);
    /* 2. edit action */
    if ((cname == "QuLabel") || (cname == "QuLed") || cname == "QuTable")
         list.append(d_editAction);
    return list;
}

QAction *TaskMenuExtension::preferredEditAction() const
{
    return d_editConnectionAction;
}

void TaskMenuExtension::setupSourceTargetDialog(QWidget *cb_widget)
{
    QString source = cb_widget->property("source").toString();
    QString targets = cb_widget->property("targets").toString();

    QDialog *w = new QDialog();
    //	QDialog *wt = new QDialog();

    QGridLayout *grid = new QGridLayout(w);
    QTabWidget *tw = new QTabWidget(w);

    EditSourceDialog *wins = new EditSourceDialog(0);
    EditTargetDialog *wint = new EditTargetDialog(0);
    tw->addTab(wins, "Source");
    tw->addTab(wint, "Target");
    tw->setCurrentIndex(0);

    /* Creating source dialog */
    wins->ui.okButton->setHidden(true);
    wins->ui.cancelButton->setHidden(true);
    wins->ui.lineEdit->setText(source);

    /* Creating target dialog */
    wint->ui.okButton->setHidden(true);
    wint->ui.cancelButton->setHidden(true);
    wint->ui.listWidget->addItems(targets.split(";",QString::SkipEmptyParts));

    grid->setRowStretch(0, 2);
    grid->addWidget(tw, 0, 0, 1, 2);

    QPushButton *okb = new QPushButton("OK", w);
    QPushButton *cancb = new QPushButton("Cancel", w);
    grid->addWidget(okb, 1, 0);
    grid->addWidget(cancb, 1, 1);
    connect(okb, SIGNAL(clicked() ), w, SLOT(accept() ) );
    connect(cancb, SIGNAL(clicked() ), w, SLOT(reject() ) );

    if (w->exec() == QDialog::Accepted)
    {
        QDesignerFormWindowInterface *formWindow = 0;
        formWindow = QDesignerFormWindowInterface::findFormWindow(d_widget);
        formWindow->cursor()->setProperty("source", wins->ui.lineEdit->text());

        QString targets;

        QList<QListWidgetItem *>itemList = wint->ui.listWidget->findItems("*", Qt::MatchWildcard);
        for(int i = 0; i < itemList.size(); i++)
        {
            QListWidgetItem *it = itemList.at(i);
            targets.append(it->text());
            if(i < itemList.size() - 1)
                targets += ";";
        }
        formWindow->cursor()->setProperty("targets", targets);
    }
    else
        qDebug() << "Caso non accettato!";

    delete wins;
    delete wint;
    delete w;
}

void TaskMenuExtension::editConnection()
{
    QString src;
    bool edit_source = true;
    /* beware: TLabel after TReaderWriter because TReaderWriter IS a TLabel! */
    if (QuLabel *qtl = qobject_cast<QuLabel*>(d_widget))
        src = qtl->source();
    else if (QuLed *qled = qobject_cast<QuLed*>(d_widget))
        src = qled->source();
    else if(QuTable *t = qobject_cast<QuTable *>(d_widget))
        src = t->source();
    else if(QuTrendPlot *t = qobject_cast<QuTrendPlot *>(d_widget))
        src = t->source();
    else if(QuSpectrumPlot *t = qobject_cast<QuSpectrumPlot *>(d_widget))
        src = t->source();
    else if(QuButton *b = qobject_cast<QuButton *>(d_widget))
    {
        src = b->targets();
        edit_source = false;
    }
    else if(QuApplyNumeric *an = qobject_cast<QuApplyNumeric *>(d_widget))
    {
        src = an->targets();
        edit_source = false;
    }

    else
        return;

    if (edit_source)
    {
        if (editSourceDialog == 0)
            editSourceDialog = new EditSourceDialog();

        editSourceDialog->ui.lineEdit->setText(src);
        if (editSourceDialog->exec() == QDialog::Accepted)
        {
            QDesignerFormWindowInterface *formWindow = 0;
            formWindow = QDesignerFormWindowInterface::findFormWindow(d_widget);
            formWindow->cursor()->setProperty("source", editSourceDialog->ui.lineEdit->text());
        }
    }
    else
    {
        EditTargetDialog *d = new EditTargetDialog();
        d->ui.listWidget->addItems(src.split(';'));
        if (d->exec() == QDialog::Accepted)
        {
            QDesignerFormWindowInterface *formWindow = 0;
            formWindow = QDesignerFormWindowInterface::findFormWindow(d_widget);
            QString targets;
            QList<QListWidgetItem *>itemList = d->ui.listWidget->findItems("*", Qt::MatchWildcard);
            for(int i = 0; i < itemList.size(); i++)
            {
                QListWidgetItem *it = itemList.at(i);
                targets.append(it->text());
                if(i < itemList.size() - 1)
                    targets += ";";
            }
            formWindow->cursor()->setProperty("targets", targets);
        }
    }
}

void TaskMenuExtension::editAttributes()
{
    if (QuLabel *label = qobject_cast<QuLabel*>(d_widget))
    {
        TLabelBoolEditor *w = new TLabelBoolEditor(label->property("trueString").toString(),
                                                   label->property("falseString").toString(),
                                                   label->property("trueColor").value<QColor>(),
                                                   label->property("falseColor").value<QColor>());

        if (w->exec() == QDialog::Accepted)
        {
            QDesignerFormWindowInterface *formWindow = 0;
            formWindow = QDesignerFormWindowInterface::findFormWindow(label);
            formWindow->cursor()->setProperty("trueString", w->ui.lineEditTrue->text());
            formWindow->cursor()->setProperty("falseString", w->ui.lineEditFalse->text());
            formWindow->cursor()->setProperty("trueColor", w->ui.pushColorTrue->palette().color(QPalette::Button));
            formWindow->cursor()->setProperty("falseColor", w->ui.pushColorFalse->palette().color(QPalette::Button));
        }
    }
    else if (QuTable *table = qobject_cast<QuTable*>(d_widget))
    {
        TTableBoolEditor *w = new TTableBoolEditor(table->readNumRows(), table->readNumColumns(), table->trueStrings(), table->falseStrings(), table->trueColors(), table->falseColors(), table->getDisplayMask());

        if (w->exec() == QDialog::Accepted)
        {
            QDesignerFormWindowInterface *formWindow = 0;
            formWindow = QDesignerFormWindowInterface::findFormWindow(table);
            formWindow->cursor()->setProperty("numRows", w->ui.eFlagTrue->readNumRows());
            formWindow->cursor()->setProperty("numColumns", w->ui.eFlagTrue->readNumColumns());
            formWindow->cursor()->setProperty("trueStrings", w->ui.eFlagTrue->trueStrings());
            formWindow->cursor()->setProperty("falseStrings", w->ui.eFlagTrue->falseStrings());
            formWindow->cursor()->setProperty("trueColors", w->ui.eFlagTrue->trueColors());
            formWindow->cursor()->setProperty("falseColors", w->ui.eFlagTrue->falseColors());
            formWindow->cursor()->setProperty("displayMask", w->ui.lineMask->text());
        }
    }
}

QuLabelInterface::QuLabelInterface(QObject* parent, CumbiaPool *cumbia_p, const CuControlsFactoryPool& ctrl_factory_p)
    : CuCustomWidgetInterface(parent, cumbia_p, ctrl_factory_p)
{
    d_name = "QuLabel";
    d_include = "qulabel.h";
    d_icon = QPixmap(":pixmaps/elabel.png");
    d_domXml =
            "<widget class=\"QuLabel\" name=\"quLabel\">\n"
            " <property name=\"geometry\">\n"
            "  <rect>\n"
            "   <x>0</x>\n"
            "   <y>0</y>\n"
            "   <width>120</width>\n"
            "   <height>40</height>\n"
            "  </rect>\n"
            " </property>\n"
            "</widget>\n";
}

QWidget* QuLabelInterface::createWidget(QWidget* parent)
{
    QuLabel * label = new QuLabel(parent, cumbia_pool, ctrl_factory_pool);
    // label->setDesignerMode(true);
    DropEventFilter *dropEventFilter = new DropEventFilter(label);
    label->installEventFilter(dropEventFilter);
    return label;
}

QuButtonInterface::QuButtonInterface(QObject *parent, CumbiaPool *cumbia_p, const CuControlsFactoryPool &ctrl_factory_p)
    : CuCustomWidgetInterface(parent, cumbia_p, ctrl_factory_p)
{
    d_name = "QuButton";
    d_include = "qubutton.h";
    d_icon = QPixmap(":pixmaps/epushbutton.png");
    d_domXml =
            "<widget class=\"QuButton\" name=\"quButton\">\n"
            " <property name=\"geometry\">\n"
            "  <rect>\n"
            "   <x>0</x>\n"
            "   <y>0</y>\n"
            "   <width>60</width>\n"
            "   <height>20</height>\n"
            "  </rect>\n"
            " </property>\n"
            "</widget>\n";
}

QWidget *QuButtonInterface::createWidget(QWidget *parent)
{
    QuButton * button = new QuButton(parent, cumbia_pool, ctrl_factory_pool);
   // label->setDesignerMode(true);
    DropEventFilter *dropEventFilter = new DropEventFilter(button);
    button->installEventFilter(dropEventFilter);
    return button;
}

QuApplyNumericInterface::QuApplyNumericInterface(QObject* parent, CumbiaPool *cumbia_p, const CuControlsFactoryPool &ctrl_factory_p)
    : CuCustomWidgetInterface(parent, cumbia_p, ctrl_factory_p)
{
    d_name = "QuApplyNumeric";
    d_include = "quapplynumeric.h";
    d_icon = QPixmap(":pixmaps/eapplynumeric.png");
    d_domXml =
            "<widget class=\"QuApplyNumeric\" name=\"quApplyNum\">\n"
            " <property name=\"geometry\">\n"
            "  <rect>\n"
            "   <x>0</x>\n"
            "   <y>0</y>\n"
            "   <width>120</width>\n"
            "   <height>40</height>\n"
            "  </rect>\n"
            " </property>\n"
            "</widget>\n";
}

QWidget* QuApplyNumericInterface::createWidget(QWidget* parent)
{
    QuApplyNumeric * an = new QuApplyNumeric(parent, cumbia_pool, ctrl_factory_pool);
   // label->setDesignerMode(true);
    DropEventFilter *dropEventFilter = new DropEventFilter(an);
    an->installEventFilter(dropEventFilter);
    return an;
}

QuLedInterface::QuLedInterface(QObject *parent, CumbiaPool *cumbia_p, const CuControlsFactoryPool &ctrl_factory_p)
    : CuCustomWidgetInterface(parent, cumbia_p ,ctrl_factory_p)
{
    d_name = "QuLed";
    d_include = "quled.h";
    d_icon = QPixmap(":pixmaps/eled.png");
    d_domXml =
            "<widget class=\"QuLed\" name=\"quLed\">\n"
            " <property name=\"geometry\">\n"
            "  <rect>\n"
            "   <x>0</x>\n"
            "   <y>0</y>\n"
            "   <width>20</width>\n"
            "   <height>20</height>\n"
            "  </rect>\n"
            " </property>\n"
            "</widget>\n";
}

QWidget *QuLedInterface::createWidget(QWidget *parent)
{
    QuLed *led = new QuLed(parent, cumbia_pool, ctrl_factory_pool);
    DropEventFilter *dropEventFilter = new DropEventFilter(led);
    led->installEventFilter(dropEventFilter);
    return led;
}

QuLineEditInterface::QuLineEditInterface(QObject *parent, CumbiaPool *cumbia_p, const CuControlsFactoryPool &ctrl_factory_p)
 : CuCustomWidgetInterface(parent, cumbia_p ,ctrl_factory_p)
{
    d_name = "QuLineEdit";
    d_include = "qulineedit.h";
    d_icon = QPixmap(":pixmaps/elineedit.png");
    d_domXml =
            "<widget class=\"QuLineEdit\" name=\"quLineEdit\">\n"
            " <property name=\"geometry\">\n"
            "  <rect>\n"
            "   <x>0</x>\n"
            "   <y>0</y>\n"
            "   <width>120</width>\n"
            "   <height>40</height>\n"
            "  </rect>\n"
            " </property>\n"
            "</widget>\n";
}

QWidget *QuLineEditInterface::createWidget(QWidget *parent)
{
    QuLineEdit *le = new QuLineEdit(parent);
    DropEventFilter *dropEventFilter = new DropEventFilter(le);
    le->installEventFilter(dropEventFilter);
    return le;
}

QuTableInterface::QuTableInterface(QObject *parent, CumbiaPool *cumbia_p, const CuControlsFactoryPool &ctrl_factory_p)
    : CuCustomWidgetInterface(parent, cumbia_p, ctrl_factory_p)
{
    d_name = "QuTable";
    d_include = "qutable.h";
    d_icon = QPixmap(":pixmaps/eflag.png");
    d_domXml =
            "<widget class=\"QuTable\" name=\"quTable\">\n"
            " <property name=\"geometry\">\n"
            "  <rect>\n"
            "   <x>0</x>\n"
            "   <y>0</y>\n"
            "   <width>120</width>\n"
            "   <height>240</height>\n"
            "  </rect>\n"
            " </property>\n"
            "</widget>\n";
}

QWidget *QuTableInterface::createWidget(QWidget *parent)
{
    QuTable *t = new QuTable(parent, cumbia_pool, ctrl_factory_pool);
    DropEventFilter *dropEventFilter = new DropEventFilter(t);
    t->installEventFilter(dropEventFilter);
    return t;
}

QuTrendPlotInterface::QuTrendPlotInterface(QObject *parent, CumbiaPool *cumbia_p, const CuControlsFactoryPool &ctrl_factory_p)
    : CuCustomWidgetInterface(parent, cumbia_p, ctrl_factory_p)
{
    d_name = "QuTrendPlot";
    d_include = "qutrendplot.h";
    d_icon = QPixmap(":pixmaps/eplotlight.png");
    d_domXml =
            "<widget class=\"QuTrendPlot\" name=\"quTrendPlot\">\n"
            " <property name=\"geometry\">\n"
            "  <rect>\n"
            "   <x>0</x>\n"
            "   <y>0</y>\n"
            "   <width>360</width>\n"
            "   <height>240</height>\n"
            "  </rect>\n"
            " </property>\n"
            "</widget>\n";
}

QWidget *QuTrendPlotInterface::createWidget(QWidget *parent)
{
    QuTrendPlot *t = new QuTrendPlot(parent, cumbia_pool, ctrl_factory_pool);
    DropEventFilter *dropEventFilter = new DropEventFilter(t);
    t->installEventFilter(dropEventFilter);
    return t;
}

QuSpectrumPlotInterface::QuSpectrumPlotInterface(QObject *parent, CumbiaPool *cumbia_p, const CuControlsFactoryPool &ctrl_factory_p)
    : CuCustomWidgetInterface(parent, cumbia_p, ctrl_factory_p)
{
    d_name = "QuSpectrumPlot";
    d_include = "quspectrumplot.h";
    d_icon = QPixmap(":pixmaps/eplotlight.png");
    d_domXml =
            "<widget class=\"QuSpectrumPlot\" name=\"quSpectrumPlot\">\n"
            " <property name=\"geometry\">\n"
            "  <rect>\n"
            "   <x>0</x>\n"
            "   <y>0</y>\n"
            "   <width>360</width>\n"
            "   <height>240</height>\n"
            "  </rect>\n"
            " </property>\n"
            "</widget>\n";
}

QWidget *QuSpectrumPlotInterface::createWidget(QWidget *parent)
{
    QuSpectrumPlot *t = new QuSpectrumPlot(parent, cumbia_pool, ctrl_factory_pool);
    DropEventFilter *dropEventFilter = new DropEventFilter(t);
    t->installEventFilter(dropEventFilter);
    return t;
}
