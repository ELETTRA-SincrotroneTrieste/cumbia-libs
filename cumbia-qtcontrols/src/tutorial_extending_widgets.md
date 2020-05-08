# Writing plugins to add new widgets to *cumbia*. {#tutorial_extending_widgets}

In this tutorial we will learn how to write a plugin exporting a set of widgets that share the same interface as that exposed by the *cumbia-qtcontrols* elements.
A client of the plugin will then instantiate an object and use it as any other *cumbia-qtcontrols* widget.


## Realtime plot plugin

A common pattern at Elettra synchrotron is to have Tango devices exposing commands with two input arguments that retur arrays of data. They are usually read with a frequency of 10Hz. The first argument specifies the *mode*, the second the number of desired elements.
The mode is usually fixed and decided by the programmer, but the user may want to change the reading frequency and the number of elements.
The plugin will extend QuSpectrumPlot and provide the controls to operate this changes. They are offered by an action added to the right click context menu.

### 1. Write the widget code

First, we start writing the code for the widget.
The RTPlot *is a* QuSpectrumPlot and inherits from the QuXtraWidgetI interface:

```cpp
#include <quspectrumplot.h>
#include <cudata.h>
#include <cudatalistener.h>
#include <quxtrawidgetplugininterface.h>

class Cumbia;
class CuControlsReaderFactoryI;

class QuRTPlot2 : public QuSpectrumPlot, public QuXtraWidgetI
{
public:
    Q_OBJECT
    QuRTPlot2(QWidget *parent, Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac);
    QuRTPlot2(QWidget *parent, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool);

    ~QuRTPlot2();
    
    // QuXtraWidgetI interface
public:
    CuContext *getContext() const;
    QString link() const;
    void setLink(const QString &s);
    void unsetLink();
    Type getType() const;
};
```

The QuRTPlot constructor takes the same input arguments as the QuSpectrumPlot, so that the implementation is simple 
(example for the first constructor only). Enable Y axis auto scaling by default:

```cpp
QuRTPlot2::QuRTPlot2(QWidget *parent, Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac)
 : QuSpectrumPlot (parent, cumbia, r_fac)  {
    setYAxisAutoscaleEnabled(true);
}
```

The other methods that must be implemented from the QuXtraWidgetI interface are very easy to write:

```cpp
CuContext *QuRTPlot2::getContext() const {
    return QuSpectrumPlot::getContext();
}

QString QuRTPlot2::link() const {
    return QuSpectrumPlot::source();
}

void QuRTPlot2::setLink(const QString &s) {
    QuSpectrumPlot::setSource(s);
}

void QuRTPlot2::unsetLink() {
    QuSpectrumPlot::unsetSource(QString());
}

QuXtraWidgetI::Type QuRTPlot2::getType() const {
    return  QuXtraWidgetI::Reader;
}

```

If we want to allow setting multiple sources through a list of strings, as QuSpectrumPlot does, we can provide a convenient
*Qt property* in order to let the clients of QuRTPlot2 access the *setSources* and *sources* methods without need for *dynamic_cast* at runtime:

```cpp
class QuRTPlot2 : public QuSpectrumPlot, public QuXtraWidgetI
{
    Q_OBJECT
    Q_PROPERTY(QStringList sources READ sources WRITE setSources)

    // ...
      QStringList sources() const;

public slots:
    void setSources(const QStringList& srcs);
   // ...
}
```

The two added methods are just shortcuts calling the base class equivalents:

```cpp
QStringList QuRTPlot2::sources() const {
    return QuSpectrumPlot::sources();
}

void QuRTPlot2::setSources(const QStringList &srcs) {
    QuSpectrumPlot::setSources(srcs);
}
```

A dialog will provide options to change the period and the number of samples. It is accessible through the contextual menu.
We must reimplement *contextMenuEvent* and extend the menu with a proper action.
QuSpectrumPlot, as well as QuTrendPlot, use the *strategy* pattern to create contextual menus. Get the QuSpectrumPlot *context menu
stragegy*, let it create the menu and extend it with a new action:

```cpp

protected:
    void contextMenuEvent(QContextMenuEvent *e);
```

Implementation:
```
#include <QMenu>
// ...

void QuRTPlot2::contextMenuEvent(QContextMenuEvent *e) {
    QMenu *menu = contextMenuStrategy()->createMenu(this);
    menu->addAction("Configure real time plot...", this, SLOT(configureRT()));
    menu->exec(QCursor::pos());
}
```

A configureRT() *slot* must be added. It creates a *dialog* with the necessary controls to change the period and the number of samples.
Since *Elettra* "*real time*" data is acquired through *Tango* commands with two input arguments and the syntax of the *source* is like

```cpp
	"bc01/diagnostics/rtcm_bc01.01->GetCharge(0,1000)"
```

a *regular expression* is employed to replace the old source with the new one. Other implementations are possible.
A *QDialog* is populated with two *QLabel*s, two *QSpinBox*es and two *QPushButton*s.

```cpp
void QuRTPlot2::configureRT() {
    QDialog d(this);
    QGridLayout *lo = new QGridLayout(&d);
    lo->setMargin(3);
    lo->setSpacing(lo->spacing() / 3);
    QLabel *lperiod = new QLabel("Period", &d);
    lperiod->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    QLabel *lnsam = new QLabel("Number of samples", &d);
    lnsam->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    QSpinBox *sbperiod = new QSpinBox(&d);
    sbperiod->setMinimum(10);
    sbperiod->setMaximum(10000);
    sbperiod->setValue(100);
    sbperiod->setSuffix("ms");
    sbperiod->setObjectName("sbperiod");
    QSpinBox *sbSamples = new QSpinBox (&d);
    sbSamples->setMinimum(10);
    sbSamples->setMaximum(1000000);
    sbSamples->setValue(1000);
    sbSamples->setObjectName("sbnsam");

    lo->addWidget(lperiod, 0, 0, 1, 2);
    lo->addWidget(sbperiod, 0, 2, 1, 2);
    lo->addWidget(lnsam, 0, 4, 1, 2);
    lo->addWidget(sbSamples, 0, 6, 1, 2);

    QPushButton *pbClose = new QPushButton("Close", &d);
    pbClose->setObjectName("pbClose");
    connect(pbClose, SIGNAL(clicked()), &d, SLOT(reject()));
    lo->addWidget(pbClose, 1, 0, 1, 1);
    QPushButton *pbApply = new QPushButton("Apply", &d);
    pbApply->setObjectName("pbApply");
    lo->addWidget(pbApply, 1, 7, 1, 1);
    connect(pbApply, SIGNAL(clicked()), &d, SLOT(accept()));

    if(d.exec() == QDialog::Accepted) {
        int timeout = sbperiod->value();
        int nsam = sbSamples->value();
        QStringList srcs;
        if(timeout != period())
            setPeriod(timeout);
        foreach(QString s, sources()) {
            QRegExp cmdRe("(.*)\\((\\d+),(\\d+)\\)");
            if(cmdRe.indexIn(s) > -1 && cmdRe.capturedTexts().size() > 3) {
                QStringList caps = cmdRe.capturedTexts();
                QString newSrc = QString("%1(%2,%3)").arg(caps[1]).arg(caps[2]).arg(nsam);
                srcs << newSrc;
            }
        }
        if(!srcs.isEmpty() && srcs != sources())
            setSources(srcs);
    }
}
```

The widget is now ready. The context menu on the plot allows to configure the new *real time plot*.
In the next section, we have to write the *plugin* code.

### 2. The plugin code

In order to be loaded as an *extra widget plugin*, the name of the plugin must meet the requirements discussed in the
QuXtraWidgetPluginLoader documentation.

- *somename-xwidgetplugin.so*

is an example of valid name. We choose *elettra.eu.realtimeplot-xwidgetplugin* for this example.

### 2a. The Qt project file

In the project file, we *include* *cumbia-qtcontrols.pri* project include file to ensure the plugin
can see the necessary include files and can link to *cumbia-qtcontrols* library.
We install the plugin under *$${INSTALL_ROOT}/include/qumbia-plugins* and alongside the plugin library
we distribute *qurtplot2.h* so that clients can access its functions, if needed.
A good practise is to define *Qt properties (Q_PROPERTY)* to access plugin widget properties instead
(this avoids *dynamic_cast*).


```cpp
include(/usr/local/cumbia-libs/include/cumbia-qtcontrols/cumbia-qtcontrols.pri)

QT       += core gui

TARGET = elettra.eu.realtimeplot-xwidgetplugin
TEMPLATE = lib
CONFIG += plugin

SOURCES += \
        src/elettra.eu.realtimeplot-xwidgetplugin.cpp \
    src/qurtplot2.cpp

HEADERS += \
        src/elettra.eu.realtimeplot-xwidgetplugin.h \
    src/qurtplot2.h

INC_PATH = $${INSTALL_ROOT}/include/qumbia-plugins
inc.files = src/qurtplot2.h
inc.path = $${INC_PATH}

DISTFILES += elettra.eu.realtimeplot-xwidgetplugin.json

unix {
    target.path = $${DEFINES_CUMBIA_QTCONTROLS_PLUGIN_DIR}
    INSTALLS += target inc
}

message("elettra.eu.realtimeplot-xwidgetplugin: plugin installation dir:  $${DEFINES_CUMBIA_QTCONTROLS_PLUGIN_DIR}")
message("elettra.eu.realtimeplot-xwidgetplugin: include installation dir: $${INC_PATH}")

```

### 2b. The plugin files

The header file, named elettra.eu.realtimeplot-xwidgetplugin.h, contains the *RTPlotXWidgetPlugin* class, implementing the QuXtraWidgetPluginI interface 
from *cumbia-qtcontrols* quxtrawidgetplugininterface.h. As you can see, the code is very easy to write.
With *qtcreator*, once defined the class

```cpp
class RTPlotXWidgetPlugin : public QObject, public QuXtraWidgetPluginI
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QGenericPluginFactoryInterface" FILE "elettra.eu.realtimeplot-xwidgetplugin.json")

public:
    explicit RTPlotXWidgetPlugin(QObject *parent = nullptr);
    ~RTPlotXWidgetPlugin();

    Q_INTERFACES(QuXtraWidgetPluginI)
};
```

right click on the class name, *RTPlotXWidgetPlugin*, and choose *Refactor* --> *Insert Virtual Functions of Base Classes* to get the skeleton
of the method definitions (in .h file) and implementation (in .cpp file):

```cpp
#ifndef RTPLOTXWIDGETPLUGIN_H
#define RTPLOTXWIDGETPLUGIN_H

#include <quxtrawidgetplugininterface.h>
#include <QObject>

class RTPlotXWidgetPlugin : public QObject, public QuXtraWidgetPluginI
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QGenericPluginFactoryInterface" FILE "elettra.eu.realtimeplot-xwidgetplugin.json")

public:
    explicit RTPlotXWidgetPlugin(QObject *parent = nullptr);
    ~RTPlotXWidgetPlugin();

    Q_INTERFACES(QuXtraWidgetPluginI)

    // QuXtraWidgetPluginI interface
    QWidget *create(const QString &name, QWidget *parent, Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac);
    QWidget *create(const QString &name, QWidget *parent, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool);
    QString name() const;
    QStringList catalogue() const;
};
```

The key methods to implement are the two *create* ones: according to the given class name, they instantiate one of the objects available in the plugin
*catalogue*. In this example plugin, only *QuRTPlot2* widget is offered. In the real *elettra.eu.realtimeplot-xwidgetplugin*, another version of plot
is provided.

```cpp

// include files
#include "elettra.eu.realtimeplot-xwidgetplugin.h"
#include <cucontrolsreader_abs.h>
#include <cucontext.h>
#include "qurtplot.h"
#include "qurtplot2.h"
#include <cumacros.h>


// create methods
//
QWidget *RTPlotXWidgetPlugin::create(const QString &name, QWidget *parent, Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac)
{
    if(name == "QuRTPlot2")
        return new QuRTPlot2(parent, cumbia, r_fac);
    perr("RTPlotXWidgetPlugin.create: RTPlotXWidgetPlugin does not provide the widget \"%s\"", qstoc(name));
    return nullptr;
}

QWidget *RTPlotXWidgetPlugin::create(const QString &name, QWidget *parent, CumbiaPool *cumbia_pool,
                                                const CuControlsFactoryPool &fpool)
{
    if(name == "QuRTPlot2")
        return new QuRTPlot2(parent, cumbia_pool, fpool);
    perr("RTPlotXWidgetPlugin.create: RTPlotXWidgetPlugin does not provide the widget \"%s\"", qstoc(name));
    return nullptr;
}
```

A *catalogue* method is required by the plugin interface in order to provide the list of classes (widgets) provided by the plugin.
In this example, only one widget is offered:

```cpp
QStringList RTPlotXWidgetPlugin::catalogue() const
{
    return QStringList() << "QuRTPlot2";
}
```

The *catalogue* is employed by the QuXtraWidgetPluginLoader to inform the client whether a given class is available or not.
The QuXtraWidgetPluginLoader scans all *extra widget* plugins for a given class name.

Two last methods must be implemented, *name* and *description*. The code is self explanatory:

```cpp
QString RTPlotXWidgetPlugin::name() const
{
    return "elettra.eu.rtplotxwidgetplugin";
}

QString RTPlotXWidgetPlugin::description() const
{
    return QString("%1 provides a \"realtime plots\" that connect and display arrays "
                   "from some special Elettra Tango devices exporting \"real time\" commands.").arg(name());
}
```

### 3. The client.

The client is an application which skeleton can be generated with the utility

```bash
cumbia new project
```

In this example, the code to load the plugin and instantiate the *real time plot* is written in the constructor.
First, we get the plugin that provides the class we need, *QuRTPlot2* and, if available, we ask the instantiation of the object:

```cpp
#include "RTPlotXWPluginExample.h"

// cumbia-tango
#include <cuserviceprovider.h>
#include <cumacros.h>
// cumbia-tango

#include <QGridLayout>
#include <QVariant>
#include <quxtrawidgetpluginloader.h>

RTPlotXWPluginExample::RTPlotXWPluginExample(CumbiaTango *cut, QWidget *parent) :
    QWidget(parent)
{
    // cumbia-tango
    cu_t = cut;
    m_log = new CuLog(&m_log_impl);
    cu_t->getServiceProvider()->registerService(CuServices::Log, m_log);
    // cumbia-tango

    QGridLayout *lo = new QGridLayout(this);
    QuXtraWidgetPluginLoader xwpl;
    QuXtraWidgetPluginI *pi = xwpl.getPlugin("QuRTPlot2");
    if(pi) {
        QWidget *rtPlot = pi->create("QuRTPlot2", parent, cut, this->cu_tango_r_fac);
	if(rtPlot) {
	        lo->addWidget(rtPlot);
        	rtPlot->setObjectName("rtPlot");
	        // either use the "source" property
        	rtPlot->setProperty("source", qApp->arguments()[1]);
        	// or
        	// dynamic_cast<QuXtraWidgetI *>(rtPlot)->setLink(qApp->arguments().at(1));
	} // else: very strange
    } // else error: the plugin is not [correctly] installed
}
```

If everything is as expected, build the plugin, then the client app. You'll get a plot that not only displays arrays from 
*elettra real time* devices, but also configures the polling period and the number of samples through a *popup dialog*















