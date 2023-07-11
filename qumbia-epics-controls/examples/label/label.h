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

class MyReadLabel : public QLineEdit , public CuDataListener
{
    Q_OBJECT
private:
    CuControlsReaderA *m_reader;

public:
    MyReadLabel(QWidget* parent, Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac)
        :QLineEdit(parent) {
        m_reader = r_fac.create(cumbia, this);
        setReadOnly(true);
    }

    virtual ~MyReadLabel() {
        pdelete("~MyReadLabel"); // print ~MyReadLabel
        delete m_reader;
    }

    void setSource(const QString& src) {
        m_reader->setSource(src);
    }

    void contextMenuEvent(QContextMenuEvent *) {
        QString src = QInputDialog::getText(this, "Change source", // title
                                            "Type the new source: ", // label
                                            QLineEdit::Normal, // echo mode
                                            m_reader->source()); // initial string
        setSource(src);
    }

    // CuDataListener interface
public:
    void onUpdate(const CuData &data) {
        bool read_ok = !data[CuDType::Err].toBool();  // data["err"]
        setEnabled(read_ok);
        // tooltip with message from Tango
        setToolTip(data[CuDType::Message].toString().c_str());  // data["msg"]

        if(!read_ok)
            setText("####");
        else if(data.containsKey(CuDType::Value))  {  // data.containsKey("value")
            CuVariant val = data[CuDType::Value];  // data["value"]
            QString txt = QString::fromStdString(val.toString());
            setText(txt);
        }
    }
};

class WidgetWithLabel : public QWidget {
    Q_OBJECT
public:
    explicit WidgetWithLabel(CumbiaEpics *cu_epi, QWidget *parent = 0) : QWidget(parent ) {
        cu_ep = cu_epi;
        QVBoxLayout *lo = new QVBoxLayout(this); // This is Qt widget layout
        QLabel *lab = new QLabel(qApp->arguments().at(1), this); // Qt label with src name
        // MyReadLabel is not Tango aware but will receive data from Tango
        CuEpReaderFactory rf;
        // Create label and inject Tango aware Cumbia and reader factory
        MyReadLabel *l = new MyReadLabel(this, cu_ep, rf);
        l->setSource(qApp->arguments().at(1)); // connect!

        // Qt:: add label and MyLabel to layout
        lo->addWidget(lab);
        lo->addWidget(l);
    }

private:
    CumbiaEpics *cu_ep;
};

#endif // CUTCWIDGETS_H

