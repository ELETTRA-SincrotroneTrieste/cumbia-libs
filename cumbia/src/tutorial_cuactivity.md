# Writing a simple activity {#tutorial_cuactivity}

In this tutorial, we will learn how to write an activity that executes some work in the background and 
delivers the result to the main thread (the graphical user interface's thread).
The code can be found under the *cumbia-qtcontrols/examples/simple_activity* directory of the cumbia-qtcontrols library sources.
We chose to write a Qt application to make things easier, since Qt QApplication provides its own *event loop*
where events from the cumbia activities are posted and processed. 
To use the QApplication *event loop*, cumbia needs a class that is available in that module. In fact, the cumbia *base* library
does not depend on Qt and does not know anything about Qt *event loop*. 
For this reason, the example resides under the cumbia-qtcontrols module, even though it is thought to guide the reader through
the learning of the *cumbia base* module only.

We can create and edit the project with *qtcreator*. In the example, the project needs to be linked with
the cumbia libraries and the include path must contain cumbia headers.
Since the project needs a cumbia-qtcontrols class to interact with the QApplication *event loop*, just include 
the *cumbia-qtcontrols.pri project include* file made available by the *cumbia-qtcontrols* library installation.

\code
include(/usr/local/include/cumbia-qtcontrols/cumbia-qtcontrols.pri)
\endcode

If the project is generated with *qtcreator*, the .pro file will look like this, after we add the *include* directive
at the top

\code

include(/usr/local/include/cumbia-qtcontrols/cumbia-qtcontrols.pri)

TEMPLATE = app
CONFIG += c++11
CONFIG -= app_bundle
CONFIG += qt

SOURCES += main.cpp \
    activity.cpp \
    simplea.cpp

HEADERS += \
    activity.h \
    simplea.h
    
\endcode

Two classes have been added in the activity.h (.cpp) and simplea.h (.cpp): 

\li Activity (activity.h) implements CuIsolatedActivity (an activity which *execute* method is run only once)
\li SimpleA (simplea.h) is a Qt widget that will show a progress bar, a QTreeWidget and a push button.

The Activity will simply produce some text messages in the background and deliver them
to the widget, sleeping a while between the messages.

This is the skeleton of the Activity class:

\code

#include <cuactivity.h>

class Activity : public CuIsolatedActivity
{
public:
    Activity();
};

\endcode

Right click on the class name, choose *refactor* and then *insert virtual functions of base classes*.
In the dialog box, select *insert definitions in implementation file* at the bottom.
Your activity.h file will look like this now:

\code
#include <cuisolatedactivity.h>

class Activity : public CuIsolatedActivity
{
public:
    Activity();
    
    // CuActivity interface
public:
    int getType() const;
    void event(CuActivityEvent *e);
    bool matches(const CuData &token) const;
    int repeat() const;
    
protected:
    void init();
    void execute();
    void onExit();
};

\endcode

\par The class constructor
We will use CuIsolatedActivity::CuIsolatedActivity(const CuData& token) class constructor for the base class.
The token is a CuData bundle that contains some key/value pairs describing the activity. We will see
hot to deal with it when we instantiate the activity.
As described in the CuIsolatedActivity::CuIsolatedActivity class constructor documentation, the following 
activity flags are enabled by default:

\li CuActivity::CuADeleteOnExit 
\li CuActivity::CuAUnregisterAfterExec

They are ok for this example, because cumbia will automatically unregiter and dispose the Activity when 
the job is over.

\par Implementing the CuActivity interface

Let's discuss how to implement the CuActivity interface

\li getType will return CuActivity::Isolated, because the activity executes only once.
\li we will leave the event method empty in this simple example
\li matches will return true if the token matches *this activity* token

\code

Activity::Activity(const CuData& token)
    : CuIsolatedActivity(token) {

}

int Activity::getType() const {
    return CuActivity::Isolated;
}

void Activity::event(CuActivityEvent *) {

}

bool Activity::matches(const CuData &token) const {
    return getToken() == token;
}

\endcode

\par CuActivity hooks: *init, execute, onExit*

Do now implement the three most important methods where the work is carried out by the activity.
*init, execute* and *onExit* are run in a separate thread. We don't have to worry about that, and we 
can forgive about locking and variable synchronization too. In this simple application, the methods will
produce some messages that the graphical interface will show. Each message contains the value of the 
pointer to the current thread (as returned by pthread_self() ), so that it will be clear from which thread the code
is run.

The *init* and *onExit* will prepare and deliver a text message with the name of the method and the address
of the *current thread*:

\code
void Activity::init()
{
    // result: a CuData.
    // pick the token to initialise the result
    CuData d = getToken();
    d["msg"] = std::string("init"); // insert the message into the result
    d["thread"] = write_thread_t();
    publishResult(d); // post to main thread
}

void Activity::onExit()
{
    // do exactly the same as init
    CuData d = getToken();
    d["msg"] = std::string("onExit");
    d["thread"] = write_thread_t();
    publishResult(d);
}
\endcode

The *execute* method will do its work in three stages. It will publish the progress each time.
The function will sleep for two seconds between each stage.

\code

void Activity::execute()
{
    CuData d = getToken();
    for(int i = 1; i <= 3; i++) {
        d["msg"] = std::string("execute [") + std::to_string(i) + std::string("]");
        d["thread"] = write_thread_t();
        publishProgress(i, 3, d);
        if (i < 3) sleep(2);
    }
    d["thread"] = write_thread_t();
    d["msg"] = std::string("execute [done]");
    publishResult(d);
}

\endcode

\par The graphical user interface

Let's add a QProgressBar, a QTreeWidget and a QCheckBox to the graphical user interface.
The tree widget will display all messages as soon as they are delivered.
A "start" QPushButton will launch the activity on demand.
A *start* Qt *slot* will be connected to the QPushButton *clicked* signal. It will reset
the progress bar and launch the activity.
The *checkbox*, if checked, will tell *cumbia* to run the activity in the same thread, if
unchecked, each activity is run in a separate thread.
If you click the *start* button before an activity is completed, you will notice the different
behavior of the application.

Open the simplea.cpp and create the widgets as shown below. Use a vertical box layout
to place the widgets in two rows.

\code
#include "simplea.h"
#include <QProgressBar>
#include <QTreeWidget> // display events in a list 
#include <QTreeWidgetItem> // to populate the tree widget with items
#include <QVBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QHeaderView>

SimpleA::SimpleA(QWidget *parent) : QWidget(parent)
{
   QVBoxLayout *vlo = new QVBoxLayout(this); // layout in two rows
    QProgressBar *pb = new QProgressBar(this); // progress bar
    pb->setMinimum(0); pb->setMaximum(3); pb->setValue(0);
    vlo->addWidget(pb);
    QTreeWidget *tw = new QTreeWidget(this);
    tw->setHeaderLabels(QStringList() << "activity" << "message" << "activity thread" << "main thread");
    tw->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    vlo->addWidget(tw);
    QPushButton *pbu = new QPushButton("start", this);
    connect(pbu, SIGNAL(clicked()), this, SLOT(start()));
    vlo->addWidget(pbu);
    QPushButton *pbclear = new QPushButton("clear", this);
    connect(pbclear, SIGNAL(clicked()), tw, SLOT(clear()));
    vlo->addWidget(pbclear);
    QCheckBox *cb = new QCheckBox("same thread", this);
    cb->setObjectName("cbSameTh");
    cb->setToolTip("if this is checked and \"start\" is clicked while another activity\n"
                   "is running, the next activity is started in the same thread as the\n"
                   "running one. Otherwise, another thread is started");
    cb->setChecked(true);
    vlo->addWidget(cb);
    resize(500, 400);
}

void SimpleA::start()
{
    findChild<QProgressBar *>()->setValue(0);
}
\endcode

In the simplea.h header file declare the *start* slot as *private*.

\code
#include <QWidget>

class SimpleA : public QWidget
{
    Q_OBJECT
public:
    explicit SimpleA(QWidget *parent = nullptr);

private slots:
    void start();
};
\endcode

In the *start* slot, an Activity is instantiated. It must be registered through the Cumbia::registerActivity method,
that takes a *CuThreadListener* amongst its arguments. The data from the worker thread is delivered through
CuThreadListener::onResult and CuThreadListener::onProgress. Moreover, another method from CuThreadListener must
be implemented: CuThreadListener::getToken. The token returned by this call is a CuData bundle storing key/value pairs 
used to describe the listener.

Modify the code so that SimpleA implements the CuThreadListener interface. Right click again on the class name
in the header file and  choose *refactor* and then *insert virtual functions of base classes*.

\code
#include <cuthreadlistener.h>

class SimpleA : public QWidget, public CuThreadListener
\endcode

*qtcreator* will insert the method definitions and the epty bodies for the required methods to be implemented

As aforementioned, activities are registered through Cumbia::registerActivity. Declare a Cumbia attribute in the
SimpleA class as a private member.

\code

    // CuThreadListener interface
public:
    void onProgress(int step, int total, const CuData &data);
    void onResult(const CuData &data);
    CuData getToken() const;
    
private:
    Cumbia *m_cumbia;
\endcode

Below the include directives at the top of the simplea.h file, add this:

\code
#include <QWidget>
#include <cuthreadlistener.h>

class Cumbia;

class SimpleA : public QWidget, public CuThreadListener
{
    // ...
\endcode

The include directive for cumbia will be added to the simplea.cpp file.

\par CuThreadListener interface implementation in simplea.cpp

The *onProgress* and *onResult* methods in SimpleA class will insert into the view a three column QTreeWidgetItem displaying:
\li the message from the *activity* running in background
\li the pointer to the worker thread
\li the pointer to the current thread (the graphical user interface thread, or *main thread*, as we use to say)

\code
void SimpleA::onProgress(int step, int /* total */, const CuData &data)
{
    QString thread, main_th;
    thread.sprintf("0x%lx", data["thread"].toULongInt());
    main_th.sprintf("0x%lx", pthread_self());
    findChild<QProgressBar *>()->setValue(step);
    new QTreeWidgetItem(findChild<QTreeWidget *>(),
                        QStringList() << QString::fromStdString(data["msg"].toString())
            << thread);
}

void SimpleA::onResult(const CuData &data)
{
    QString thread, main_th;
    thread.sprintf("0x%lx", data["thread"].toULongInt());
    main_th.sprintf("0x%lx", pthread_self());
    QTreeWidgetItem *tw = new QTreeWidgetItem(findChild<QTreeWidget *>(),
                        QStringList() << QString::fromStdString(data["msg"].toString())
            << thread);
    tw->setBackgroundColor(0, QColor(Qt::green));
}
\endcode

The *getToken* method returns a descriptive CuData structure with the name of the activity:

\code

CuData SimpleA::getToken() const
{
    printf("SimpleA::getToken is called\n");
    return CuData("activity", "simple_activity");
}

\endcode

\par Start the activity

Discuss now how to write the *start slot*. Every time the *start button* is clicked, a new activity is
launched and a counter is used to name the activity. If the "*same thread*" check box is selected, every
new activity is run in the same thread. This is accomplished by providing the same *thread token* to the 
Cumbia::registerActivity call. If the check box is not checked, then the *thread token* for the new
activity is different from the previous one. In fact, the function CuThread::isEquivalent returns true if the 
token the thread was created with is the same as the token assigned to the new activity. We use the
"*th_id*" value to increment the thread token or to leave it the same as before in the *start* method.

The Cumbia::registerActivity call requires the following parameters:
\li a pointer to the new activity
\li a pointer to a CuThreadListener, which SimpleA is an implementation
\li a *thread token*, used to determine wether the activity is run in an already existing thread with the same
*token* or in a new one, as discussed above
\li a *const reference to a CuThreadFactoryImplI* interface
\li a *const reference to a CuThreadsEventBridgeFactory_I* interface

The usage of the first three arguments is straightforward and has already been discussed.

The *CuThreadFactoryImplI* is an interface that defines how a thread for cumbia is created. For cumbia applications,
CuThread can be used, and the CuThreadFactoryImpl can be used to let cumbia instantiate a new CuThread or reuse a
running one according to the *token* provided

The *CuThreadsEventBridgeFactory_I* is an interface to define how the threads event bridge for cumbia is created. 
Since the example is a Qt application, we adopt QThreadsEventBridgeFactory, that makes use of Qt QApplication event loop.
This is the recommended choice for Qt applications.

The observations just made lead to the following lines of code:

\code
void SimpleA::start()
{
    m_cnt++;
    CuData th_tok("name", "simplea_thread"); // thread token
    if(!findChild<QCheckBox *>("cbSameTh")->isChecked())
        m_thid = m_cnt; // change thread id so that a new thread is used for the activity
    th_tok["th_id"] = m_thid;
    findChild<QProgressBar *>()->setValue(0);  // reset progress
    Activity *a = new Activity(CuData("name", "activity " + std::to_string(m_cnt)));
    m_cumbia->registerActivity(a, this, th_tok,
                               CuThreadFactoryImpl(),
                               QThreadsEventBridgeFactory());
}
\endcode

Finally, the code for the *main.cpp* file is shown. A QApplication is created and executed and the 
*SimpleA* main widget is instantiated and shown.

\code
#include <QApplication>
#include "simplea.h"

int main(int argc, char **argv)
{
    QApplication a(argc, argv );
    a.setApplicationName("simple_activity");
    SimpleA *w = new SimpleA(0);
    w->show();
    int ret = a.exec();
    delete w;
    return ret;
}
\endcode






