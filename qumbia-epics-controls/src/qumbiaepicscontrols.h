#ifndef QUMBIAEPICSCONTROLS_H
#define QUMBIAEPICSCONTROLS_H

#include "qumbia-epics-controls_global.h"

/*! \mainpage
 *
 * The qumbia-epics-controls module relies on cumbia-qtcontrols and cumbia-epics to
 * connect the objects provided by the former to the EPICS control system.
 *
 *
 *
 * \section related_readings Related readings
 *
 * \subsection github.io
 * <a href="https://elettra-sincrotronetrieste.github.io/cumbia-libs/index.html">cumbia-libs</a> on github.io
 *
 * \subsection tutorials Tutorials
 *
 * |Tutorials                                     | Module               |
 * |-------------------------------------------------------------------|:--------------------------:|
 * |  <a href="../../cumbia/html/tutorial_cuactivity.html">Writing a *cumbia* activity</a> | <a href="../../cumbia/html/index.html">cumbia</a> |
 * |  <a href="../../cumbia-tango/html/tutorial_activity.html">Writing an activity</a> | <a href="../../cumbia-tango/html/index.html">cumbia-tango</a> |
 * |  <a href="../../cumbia-tango/html/cudata_for_tango.html">CuData for Tango</a> | <a href="../../cumbia-tango/html/index.html">cumbia-tango</a> |
 * |  <a href="../../qumbia-tango-controls/html/tutorial_cumbiatango_widget.html">Writing a Qt widget that integrates with cumbia</a> | <a href="../../qumbia-tango-controls/html/index.html">qumbia-tango-controls</a>  |
 * |  <a href="../../cuuimake/html/cuuimake.html">Using <em>cumbia ui make</em></a> to process Qt designer UI files | <a href="../../cuuimake/html/index.html">qumbia-apps/cuuimake</a>  |
 * |  <a href="../../qumbiaprojectwizard/html/tutorial_qumbiatango.html">Writing a <em>Qt application</em> with cumbia and Tango</em></a>. |<a href="../../qumbiaprojectwizard/html/index.html">qumbia-apps/qumbiaprojectwizard</a>  |
 * |  <a href="../../qumbiaprojectwizard/html/tutorial_from_qtango.html">Porting a <em>QTango application</em> to <em>cumbia-tango</em></a>. |<a href="../../qumbiaprojectwizard/html/index.html">qumbia-apps/qumbiaprojectwizard</a>  |
 * |  <a href="../../qumbianewcontrolwizard/html/tutorial_qumbianewcontrolwizard.html"><em>cumbia new control</em></a>: quickly add a custom Qt widget to a cumbia project | <a href="../../qumbianewcontrolwizard/html/index.html">qumbia-apps/qumbianewcontrolwizard</a>  |
 * |  <a href="../../cumbia-qtcontrols/html/understanding_cumbia_qtcontrols_constructors.html">Understanding <em>cumbia-qtcontrols constructors, sources and targets</em></a> |<a href="../../cumbia-qtcontrols/html/index.html">cumbia-qtcontrols</a>. |
 *
 * \subsection cumodules Modules
 *
 * |Other *cumbia* modules  |
 * |-------------------------------------------------------------------|
 * | <a href="../../cumbia/html/index.html">cumbia module</a>. |
 * | <a href="../../cumbia-tango/html/index.html">cumbia-tango module</a>. |
 * | <a href="../../cumbia-qtcontrols/html/index.html">cumbia-qtcontrols module</a>.  |
 * | <a href="../../qumbia-tango-controls/html/index.html">cumbia-qtcontrols module</a>.  |
 * | <a href="../../cumbia-epics/html/index.html">qumbia-epics module</a>.   |
 * | <a href="../../qumbia-epics-controls/html/index.html">qumbia-epics-controls module</a>.  |
 *
 *
 * \subsection cu_apps apps
 *
 * These applications (and their documentation, that has already been mentioned in the *Tutorials* table above)
 * must be installed from the *qumbia-apps* sub-directory of the *cumbia-libs* distribution.
 * To install them, *cd* into that folder and execute:
 *
 * \code
 * qmake
 * make
 * sudo make install
 * \endcode
 *
 * Along the applications executables and documentation, two bash scripts will be installed:
 *
 * - /etc/bash_completion.d/cumbia
 * - /etc/bash/bashrc.d/cumbia.sh
 *
 * They define shortcuts for the common operations provided by the *qumbia-apps* applications as follows:
 *
 *
 * |Applications (command line)   | description                                 | app
 * |------------------------------|--------------------------------------------|:---------------:|
 * | *cumbia new project*          | create a new cumbia project               |<a href="../../qumbiaprojectwizard/html/index.html">qumbia-apps/qumbiaprojectwizard</a>  |
 * | *cumbia import*               | migrate a QTango project into cumbia      |<a href="../../qumbiaprojectwizard/html/index.html">qumbia-apps/qumbiaprojectwizard</a>  |
 * | *cumbia new control*          | write a *cumbia control* reader or writer | <a href="../../qumbianewcontrolwizard/html/index.html">qumbia-apps/qumbianewcontrolwizard</a>  |
 * | *cumbia ui make*              | run *cuuimake* to generate *qt+cumbia* ui_*.h files | <a href="../../cuuimake/html/index.html">qumbia-apps/cuuimake</a>  |
 * | *cumbia client*               | run a generic cumbia client | <a href="../../cumbia_client/html/index.html">qumbia-apps/cumbia_client</a>  |
 *
 *
 * *bash auto completion* will help you use these shortcuts: try
 *
 * \code
 * cumbia <TAB>
 * \endcode
 *
 * or
 *
 * \code
 * cumbia new <TAB>
 * \endcode
 *
 *
 * At the moment, only a *monitor* for an EPICS *process variable, pv* has been
 * implemented.
 *
 * We provide an example to realize a Qt widget that displays a value from an EPICS *pv*.
 * The widget, derived from QLineEdit, is then inserted into a main QWidget.
 *
 * Right clicking on the QLineEdit makes it possible to change the source
 * (see the contextMenuEvent method).
 * In the  *main.cpp * file, QApplication is used and the CumbiaEpics object
 * is instantiated and passed to the main widget. After QApplication::exec
 * returns, resources are freed.
 *
 * Remember to respect the order of the deletion of objects after QApplication::exec
 * returns:
 *
 * \li delete the widget first
 * \li delete the cumbia instance last
 *
 * \code
#include "label.h"
#include <QApplication>
#include <cumbiaepics.h>

int main(int argc, char  *argv[])
{
   QApplication a(argc, argv);
   if(qApp->arguments().count() > 1) {
       CumbiaEpics  *cu_epi = new CumbiaEpics(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
       WidgetWithLabel  *w = new WidgetWithLabel(cu_epi);
       w->show();
       int ret = a.exec();
       delete w;
       delete cu_epi;
       return ret;
   }
}
 * \endcode
 *
 * The rest of this simple application is written in the  *label.h * file.
 *
 * MyReadLabel extends QLineEdit and implements CuDataListener. QLineEdit is used to display
 * the value read from an EPICS  *pv *, CuDataListener::onUpdate is the callback where data is
 * received and the text on the line edit is updated.
 *
 * In MyReadLabel's constructor, CuControlsReaderFactoryI is used to create a CuControlsReaderA.
 * In the main widget's constructor, CumbiaEpics and CuEpReaderFactory parametrize
 * MyReadLabel::MyReadLabel so that the factory instantiates and gives back a CuEpControlsReader.
 * The parent widget is directly passed to QLineEdit's constructor, and the line edit is set
 * read only.
 *
 * In MyReadLabel's destructor the CuControlsReaderA is deleted.
 *
 * \code
#ifndef CUTCWIDGETS_H
#define CUTCWIDGETS_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <cudatalistener.h>
#include <QInputDialog>
#include "cucontrolsreader_abs.h"
#include <cumacros.h>
#include "cucontrolsfactories_i.h"
#include <cumbiaepics.h>
#include <QVBoxLayout>
#include <cuepcontrolsreader.h>
#include <QCoreApplication>     // for qApp->arguments

class CuData;

class MyReadLabel : public QLineEdit , public CuDataListener
{
   Q_OBJECT
private:
   CuControlsReaderA  *m_reader;

public:
   MyReadLabel(QWidget * parent, Cumbia  *cumbia, const CuControlsReaderFactoryI &r_fac)
       :QLineEdit(parent) {
       m_reader = r_fac.create(cumbia, this);
       setReadOnly(true);
   }

   virtual ~MyReadLabel() {
       delete m_reader;
   }

\endcode

The setSource method simply calls setSource on the reader.
setSource is called also from contextMenuEvent when the input dialog
returns the text with the new source from the user:

\code

   void setSource(const QString& src) {
       m_reader->setSource(src);
   }

   void contextMenuEvent(QContextMenuEvent  *) {
       QString src = QInputDialog::getText(this, "Change source", // title
                                           "Type the new source: ", // label
                                           QLineEdit::Normal, // echo mode
                                           m_reader->source()); // initial string
       setSource(src);
   }
\endcode

As a CuDataListener, the onUpdate method must be implemented by MyReadLabel.
The  *data * passed as argument contains the result of the read operation
from the EPICS process variable ( *pv *).

The data that we need in this example is as follows:
\li "err": boolean: error flag
\li "value": the value of the process variable, that we convert to string
   in order to display it in the label (val.toString())

\code
   // CuDataListener interface
public:
   void onUpdate(const CuData &data) {
       bool read_ok = !data[CuDType::Err].toBool();
       setEnabled(read_ok);
       // tooltip with message from Tango
       setToolTip(data[CuDType::Message].toString().c_str());

       if(!read_ok)
           setText("####");
       else if(data.containsKey("value"))  {
           CuVariant val = data[CuDType::Value];
           QString txt = QString::fromStdString(val.toString());
           setText(txt);
       }
   }
};

\endcode

The main widget is very simple. It derives from QWidget and contains a QLabel and a
MyReadLabel in a vertical box layout.
MyReadLabel is parametrized with the CumbiaEpics instance passed to the constructor
from the function  *main * and a CuEpReaderFactory factory to instantiate the CuEpControlsReader
member of the MyReadLabel class.
The function  *setSource * is finally called on MyReadLabel with the argument obtained from
the first command line parameter

\code
class WidgetWithLabel : public QWidget {
   Q_OBJECT
public:
   explicit WidgetWithLabel(CumbiaEpics  *cu_epi, QWidget  *parent = 0) : QWidget(parent ) {
       cu_ep = cu_epi;
       QVBoxLayout  *lo = new QVBoxLayout(this); // This is Qt widget layout
       QLabel  *lab = new QLabel(qApp->arguments().at(1), this); // Qt label with src name
       // MyReadLabel is not Tango aware but will receive data from Tango
       CuEpReaderFactory rf;
       // Create label and inject Tango aware Cumbia and reader factory
       MyReadLabel  *l = new MyReadLabel(this, cu_ep, rf);
       l->setSource(qApp->arguments().at(1)); // connect!

       // Qt:: add label and MyLabel to layout
       lo->addWidget(lab);
       lo->addWidget(l);
   }

private:
   CumbiaEpics  *cu_ep;
};

#endif // CUTCWIDGETS_H
 * \endcode
 *
 * This example can be found under
 * \code
 * qumbia-epics-controls/examples/label
 * \endcode
 *
 * The <a href="../../qumbia-tango-controls/html/md_src_tutorial_qumbiatango_app.html">Setting
 * up a graphical application with cumbia and tango</a> tutorial can help you understand how to
 * generate a skeleton *Qt application* with EPICS support.
 * You would choose *epics* in the *qumbiaprojectwizart* application (a screenshot of which is
 * shown in the tutorial linked above), within the *Support* groupbox at the top right.
 *
 *
 */
class QUMBIAEPICSCONTROLSSHARED_EXPORT QumbiaEpicsControls
{

public:
    QumbiaEpicsControls();
};

#endif // QUMBIAEPICSCONTROLS_H
