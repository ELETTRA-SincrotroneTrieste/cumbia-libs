# Platform independent apps from anywhere. {#quapps}

## Introduction

cumbia libraries have supported multiple engines since the beginning. Tango and EPICS have first benefited from
*native* support, that means they are supported on machines where they *are installed*.
Portable devices and *smart working* require more flexibility, and applications need be run from a desktop
where no specific *native* control system software is installed, from a web browser or an *Android* app.
*cumbia* introduced the first *engine independent* module with *cumbia-websocket*, relying on a specific server
designed in the context of the <a href="https://gitlab.com/PWMA/PWMA">PWMA project</a>.

*cumbia version 1.2.0* introduces even greater possibilities. The evolution of web technologies, the need for performance,
scalability and high security levels are both the spur and the objective of the *cumbia-http* module. This will be used
together with a new service that is currently under development and makes use of the features listed below.
If you want to take a peek at the latter, <a href="https://gitlab.com/PWMA/canone3">here</a> is the secret (that you may want
to keep for the moment).

### Features

- encrypted data transfer through *https*;
- scalability obtained with source multiplexing and the <a hreaf="https://www.nginx.com/">nginx</a> *High performance Load
  Balancer and Web Server;
- *event based* data exchange pattern, through the *Server Sent Events* technology in conjunction with the
  <a href="https://nchan.io/">nchan</a> module for *nginx*, offering a scalable and flexible *pub/sub* service for the
  modern web.
- secure authentication for the so called *write* operations, that potentially modify the state of the control system.

## Rationale

#### Foreword

In this document we will employ the term *engine* or *module* as synonyms, the meaning of the former being slightly more
intended as the "control system software or *engine*", the latter as a piece of software. Nonetheless a *native cumbia
module* will connect to a *native engine*, but here and there in the software sources we speak of loading a cumbia engine
or a cumbia module without too much concern.

In this document we describe how to create a Qt project supporting the multi engine pattern. If you find the title
*Platform independent* misleading, please rather concentrate on the *anywhere* word for now. Well, the aforementioned
http/SSE service is what actually makes app development *platform independent*, but since *cumbia-http* is written in
Qt, it works wherever the Qt installation is available, which again means potentially *everywhere*.
In the future, a cumbia-http module in pure C++ may become available, but for now the effort has been concentrated in Qt graphical
applications design, since there is plenty of *http/SSE* implementations in any language for other *platform independent*
technologies.

### Model

The realization of a multi engine framework and in particular of the new *cumbia-http* module
and the corresponding service which is an ongoing development was inspired by an outstanding example of multi platform software,
the <a href="https://telegram.org/">Telegram messaging app</a>. Its architecture allows to use the service from anywhere.
Android and iOS (iPhone, iPad) native apps are available, as well as Windows, Linux and FreeBSD (Qt) clients. Finally,
a web interface completes the list of available clients.

### Objective

The developer does not want to write an application using classes and method specific to an engine.
Moreover, the end user of the application does not want to know which engine the software relies on.
Additionally, the user wishes to launch the application with the same parameters unregarding where it connects to.

This is very easy and is accomplished through very few lines of code.
After that, application logic, source names and cumbia objects (for example, the *cumbia-qtcontrols* components)
remain engine (module) unaware and they are used transparently.

One option is to use the *cumbia new project* utility and to choose the *multi engine* option in the appropriate box.
It creates a skeleton project ready to be populated with graphical elements.

In this tutorial, we describe how this skeleton project is made.

## Writing a multi engine project

#### main.cpp

```cpp
#include <quapplication.h>
#include "httptest.h"

#include <cumbiapool.h>

int main(int argc, char *argv[])
{
    int ret;
    QuApplication qu_app( argc, argv );
    qu_app.setApplicationName("Test");
    qu_app.setApplicationVersion("1.0");

    CumbiaPool *cu_p = new CumbiaPool();
    HttpTest *w = new HttpTest(cu_p, NULL);
    w->show();

    ret = qu_app.exec();
    delete w;
    return ret;
}
```

#### widget.h

```cpp
#include <QWidget>
#include <qulogimpl.h>
#include <cucontrolsfactorypool.h>

namespace Ui {
class HttpTest;
}

class CuData;
class CumbiaPool;

class HttpTest : public QWidget
{
    Q_OBJECT
public:
    explicit HttpTest(CumbiaPool *cu_p, QWidget *parent = 0);
    ~HttpTest();

private:
    CumbiaPool *cu_pool;
    QuLogImpl m_log_impl;
    CuLog *m_log;
    CuControlsFactoryPool m_ctrl_factory_pool;
    Ui::HttpTest *ui;
};
```

#### widget.cpp
```cpp
#include "httptest.h"
#include "ui_httptest.h"
#include <quapps.h>

HttpTest::HttpTest(CumbiaPool *cumbia_pool, QWidget *parent) :
    QWidget(parent),
    cu_pool(cumbia_pool),
    ui(new Ui::HttpTest) {
    CuModuleLoader mloader(cumbia_pool, &m_ctrl_factory_pool, &m_log_impl);
    qDebug() << __PRETTY_FUNCTION__ << "available engines" << mloader.modules();
    ui->setupUi(this, cumbia_pool, m_ctrl_factory_pool);
}

HttpTest::~HttpTest() {
    delete ui;
}
```


