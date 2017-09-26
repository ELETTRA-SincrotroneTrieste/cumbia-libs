#ifndef CUTCWIDGETS_H
#define CUTCWIDGETS_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QInputDialog>
#include <qulogimpl.h>
#include <cudatalistener.h>
#include "cucontrolsreader_abs.h"
#include <cumacros.h>
#include <cudata.h>
#include "cucontrolsfactories_i.h"
#include <cumbiatango.h>
#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>
#include <QVBoxLayout>
#include <cutcontrolsreader.h>
#include <cutangoreadoptions.h> // polling/event
#include <QCoreApplication>     // for qApp->arguments
class CumbiaTango;

namespace Ui {
class Label;
}

class CuData;

class MyReadLabel : public QLineEdit , public CuDataListener
{
    Q_OBJECT
private:
    CuControlsReaderA *m_reader;

public:
    MyReadLabel(QWidget* parent,
                Cumbia *cumbia,
                const CuControlsReaderFactoryI &r_fac)
        :QLineEdit(parent)
    {
        m_reader = r_fac.create(cumbia, this);
    }

    virtual ~MyReadLabel()
    {
        pdelete("~MyReadLabel"); // print ~MyReadLabel
        delete m_reader;
    }

    void setSource(const QString& src)
    {
        m_reader->setSource(src);
    }

    void contextMenuEvent(QContextMenuEvent *)
    {
        QString src = QInputDialog::getText(this, "Change source", // title
                                            "Type the new source: ", // label
                                            QLineEdit::Normal, // echo mode
                                            m_reader->source()); // initial string
        setSource(src);
    }

    // CuDataListener interface
public:
    void onUpdate(const CuData &data)
    {
        bool read_ok = !data["err"].toBool();
        setEnabled(read_ok);
        // tooltip with message from Tango
        setToolTip(data["msg"].toString().c_str());

        if(!read_ok)
            setText("####");
        else if(data.containsKey("value"))
        {
            CuVariant val = data["value"];
            QString txt = QString::fromStdString(val.toString());
            setText(txt);
        }
    }
};

class WidgetWithLabel : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetWithLabel(QWidget *parent = 0)
    {
        cu_t = new CumbiaTango(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());

        QVBoxLayout *lo = new QVBoxLayout(this); // This is Qt widget layout
        QLabel *lab = new QLabel(qApp->arguments().at(1), this); // Qt label with src name
        // MyReadLabel is not Tango aware but will receive data from Tango
        CuTReaderFactory rf;
        // Customise read mode (optional)
        CuTangoReadOptions o(1000, CuMonitor::PolledRefresh);
        rf.setReadOptions(o);

        // Create label and inject Tango aware Cumbia and reader factory
        MyReadLabel *l = new MyReadLabel(this, cu_t, rf);
        l->setSource(qApp->arguments().at(1)); // connect!

        // Qt:: add label and MyLabel to layout
        lo->addWidget(lab);
        lo->addWidget(l);
    }

    virtual ~WidgetWithLabel(){
        delete cu_t;
    }


private:
    CumbiaTango *cu_t;
};

#endif // CUTCWIDGETS_Hgreenisthecolor

