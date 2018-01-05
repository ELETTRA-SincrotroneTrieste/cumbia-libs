#include "cuinfodialog.h"
#include <cudata.h>
#include <QMetaObject>
#include <QMetaMethod>
#include <QGroupBox>
#include <QtDebug>
#include <cumacros.h>
#include <egauge.h>
#include <cucontexti.h>
#include <culinkstats.h>
#include <cucontext.h>
#include <qutrendplot.h>
#include <QTextEdit>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QScrollArea>
#include <QGridLayout>
#include <QFrame>
#include <QPaintEvent>
#include <QPainter>
#include <QMap>
#include <eled.h>
#include <cumacros.h>
#include <qulabel.h>
#include <quspectrumplot.h>
#include <cumbia.h>
#include <cumbiapool.h>
#include <cucontrolsfactories_i.h>
#include <cucontrolsfactorypool.h>
#include <cucontrolsreader_abs.h>
#include <cucontrolswriter_abs.h>


class CuInfoDialogPrivate
{
public:
    Cumbia *cumbia;
    CumbiaPool *cu_pool;
    CuControlsFactoryPool f_pool;
    const CuControlsReaderFactoryI *r_fac;
    int layout_col_cnt;
};

CuInfoDialog::CuInfoDialog(QWidget *parent, Cumbia *cumbia, const CuControlsReaderFactoryI *r_fac)
    : QDialog(parent)
{
    d = new CuInfoDialogPrivate;
    d->cumbia = cumbia;
    d->r_fac = r_fac; // pointer copied. object not cloned
    d->cu_pool = NULL;
}

CuInfoDialog::CuInfoDialog(QWidget *parent, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool)
    : QDialog(parent)
{
    d = new CuInfoDialogPrivate;
    d->cumbia = NULL;
    d->r_fac = NULL;
    d->cu_pool = cumbia_pool;
    d->f_pool = fpool;
}

CuInfoDialog::~CuInfoDialog()
{
    // do not delete d->rfac because the reference has been
    // copied, not cloned
    delete d;
}

int CuInfoDialog::exec(QWidget *sender, CuContextI *sender_cwi)
{
    d->layout_col_cnt = 6;
    resize(700, 720);
    int row = 0;
    m_ctxwi = sender_cwi;
    m_senderw = sender;

    QFont f = font();
    f.setBold(true);
    f.setPointSize(f.pointSize() + 1);
    // update with live data
    connect(sender, SIGNAL(newData(const CuData&)), this, SLOT(onMonitorUpdate(const CuData&)));

    QString src = sender->property("source").toString();
    if(src.isEmpty())
        src = sender->property("targets").toString();
    setWindowTitle(src + " stats");
    CuLinkStats *lis = sender_cwi->getContext()->getLinkStats();

    QGridLayout *lo = new QGridLayout(this);
    // use QLabel instead of group box title to use bold font
    QLabel *lFTitleMon = new QLabel("Monitored object: \"" +
                                    sender->objectName() + "\" type \""
                                    + QString(sender->metaObject()->className()) + "\"", this);
    lo->addWidget(lFTitleMon, row, 0, 1, d->layout_col_cnt);
    lFTitleMon->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    lFTitleMon->setAlignment(Qt::AlignHCenter);
    lFTitleMon->setFont(f);

    row++;

    // operation count
    QLabel *lopcnt = new QLabel("Operation count:", this);
    QLineEdit *leopcnt = new QLineEdit(this);
    leopcnt->setObjectName("leopcnt");
    leopcnt->setReadOnly(true);
    leopcnt->setText(QString::number(lis->opCnt()));
    lo->addWidget(lopcnt, row, 0, 1, 1);
    lo->addWidget(leopcnt, row, 1, 1, 1);

    // error count
    QLabel *lerrcnt = new QLabel("Error count:", this);
    lerrcnt->setAlignment(Qt::AlignRight);
    QLineEdit *leerrcnt = new QLineEdit(this);
    leerrcnt->setObjectName("leerrcnt");
    leerrcnt->setReadOnly(true);
    leerrcnt->setText(QString::number(lis->errorCnt()));
    lo->addWidget(lerrcnt, row, 2, 1, 1);
    lo->addWidget(leerrcnt, row, 3, 1, 1);

    if(lis->errorCnt() >= 0)
    {
        QLabel *l_lasterr = new QLabel("Last err", this);
        l_lasterr->setAlignment(Qt::AlignRight);
        lo->addWidget(l_lasterr, row, 4, 1, 1);
        QLineEdit *le_lasterr = new QLineEdit(this);
        le_lasterr->setObjectName("le_lasterr");
        lo->addWidget(le_lasterr, row, 5, 1, 1);
        le_lasterr->setText(lis->last_error_msg.c_str());
    }

    row++;

    // Health
    HealthWidget *healthWidget = new HealthWidget(this);
    healthWidget->setData(lis->errorCnt(), lis->opCnt());
    lo->addWidget(healthWidget, row, 0, 1, d->layout_col_cnt);
    row++;

    int monrow = 0;
    QFrame *monitorF = new QFrame(this);
    monitorF->setObjectName("monitorF");
    QGridLayout *molo = new QGridLayout(monitorF);
    QList<CuControlsReaderA *> readers = sender_cwi->getContext()->readers();
    // create a set of GroupBoxes that will contain monitor widgets
    foreach(CuControlsReaderA *r, readers)
    {
        QGroupBox *gb = new QGroupBox(r->source(), monitorF);
        gb->setObjectName(r->source() + "_monitor");
        new QGridLayout(gb);
        molo->addWidget(gb, monrow, 0, 1, d->layout_col_cnt);
        monrow++;
    }

    // live stuff
    // Live Frame
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    QFrame *liveF = new QFrame(this);
    liveF->setObjectName("liveF");
    QGridLayout *lilo = new QGridLayout(liveF);
    foreach(CuControlsReaderA *r, readers)
    {
        QGroupBox *gb = new QGroupBox(r->source(), liveF);
        gb->setFont(f);
        lilo->addWidget(gb, row, 0, 1, d->layout_col_cnt);
        gb->setObjectName(r->source() + "_live");
        QVBoxLayout * gblilo = new QVBoxLayout(gb);
        QuLabel *llive = NULL;
        if(d->cumbia)
            llive = new QuLabel(liveF, d->cumbia, *d->r_fac);
        else if(d->cu_pool)
            llive = new QuLabel(liveF, d->cu_pool, d->f_pool);
        if(llive)
        {
            connect(llive, SIGNAL(newData(CuData)), this, SLOT(newLiveData(CuData)));
            llive->setSource(r->source());
            llive->setMaximumLength(80);
            gblilo->addWidget(llive);
        }
        row++;
    }

    // add the group boxes to the layout
    lo->addWidget(monitorF, row, 0, 1, d->layout_col_cnt); // below QLabel wit src
    row++;
    // title of second group box, the "live" one
    QLabel *lFTitleLive = new QLabel("Live reader", this);
    lFTitleLive->setFont(f);
    lFTitleLive->setAlignment(Qt::AlignHCenter);
    lFTitleLive->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    lo->addWidget(lFTitleLive, row, 0, 1, d->layout_col_cnt);
    row++;

    // add the live scroll area with the labels+plots frame inside
    scrollArea->setWidget(liveF);
    lo->addWidget(scrollArea, row, 0, 4, d->layout_col_cnt); // below monitorF
    return QDialog::exec();
}

void CuInfoDialog::onMonitorUpdate(const CuData &d)
{
    double x;
    int row = 0;
    int locolmax = 6; // layout in 6 columns
    int col = 0;
    CuLinkStats *lis = m_ctxwi->getContext()->getLinkStats();
    findChild<QLineEdit *>("leopcnt")->setText(QString::number(lis->opCnt()));
    findChild<QLineEdit *>("leerrcnt")->setText(QString::number(lis->errorCnt()));
    findChild<QLineEdit *>("le_lasterr")->setTabletTracking(lis->last_error_msg.c_str());
    HealthWidget *healthw  = findChild<HealthWidget *>();
    healthw->setData(lis->errorCnt(), lis->opCnt());

    QString src = QString(d["src"].toString().c_str());
    QGroupBox *container = findChild<QGroupBox *>(src + "_monitor");
    QGridLayout *glo = qobject_cast<QGridLayout *>(container->layout());
    d["timestamp_ms"].to<double>(x);
    if(container)
    {
        QLineEdit *lexv = container->findChild<QLineEdit *>("lexv");
        if(!lexv)
        {
            lexv = new QLineEdit(container); // timestamp
            lexv->setObjectName("lexv");
            glo->addWidget(lexv, row, 0, 1, 2); // 2 columns!
        }
        QLabel *lv = container->findChild<QLabel *>("lv");
        if(!lv) {
            lv = new QLabel("value", container); // monitored value
            lv->setObjectName("lv");
            lv->setAlignment(Qt::AlignRight);
            glo->addWidget(lv, row, 2, 1, 1);
        }
        QLineEdit *lev = container->findChild<QLineEdit *>("lev");
        if(!lev) {
            lev = new QLineEdit(container);
            lev->setObjectName("lev");
            glo->addWidget(lev, row, 3, 1, 1);
        }

        QLabel *lwv = container->findChild<QLabel *>("lwv");
        if(!lwv) {
            lwv = new QLabel("write value", container); // monitored value
            lwv->setObjectName("lwv");
            lwv->setAlignment(Qt::AlignRight);
            glo->addWidget(lwv, row, 4, 1, 1);
        }
        QLineEdit *lewv = container->findChild<QLineEdit *>("lewv");
        if(!lewv) {
            lewv = new QLineEdit(container);
            lewv->setObjectName("lewv");
            glo->addWidget(lewv, row, 5, 1, 1);
        }

        QString datetime = QDateTime::fromMSecsSinceEpoch(x).toString();
        container->findChild<QLineEdit *>("lev")->setText(d["value"].toString().c_str());
        container->findChild<QLineEdit *>("lexv")->setText(datetime);
        container->findChild<QLineEdit *>("lewv")->setEnabled(d.containsKey("w_value"));
        container->findChild<QLabel *>("lwv")->setEnabled(d.containsKey("w_value"));
        if(d.containsKey("w_value"))
            container->findChild<QLineEdit *>("lewv")->setText(d["w_value"].toString().c_str());

        row++;
        col = 0;

        QMap<QString, QString> map;
        map["activity"] = "Activity: ";
        map["data_format_str"] = "Format: ";
        map["msg"] = "Message: ";
        map["mode"] = "Mode";
        map["period"] = "Period";

        foreach(QString k, map.keys())
        {
            if(!d.containsKey(qstoc(k)))
                continue;
            if(col % locolmax == 0)
                col = 0;
            if(col == 0)
                row++;
            QLabel *l = container->findChild<QLabel *>("l_" + k);
            if(!l)
            {
                l = new QLabel(map[k], container);
                l->setObjectName("l_" + k);
                glo->addWidget(l, row, col, 1, 1);
            }
            col++;
            QLineEdit *le = container->findChild<QLineEdit *>("le_" + k);
            if(!le) {
                le = new QLineEdit(container);
                le->setObjectName("le");
                glo->addWidget(le, row, col, 1, 1);
            }
            col++;
            le->setText(d[k.toStdString()].toString().c_str());
        }

        foreach (QLineEdit *le, container->findChildren<QLineEdit*>()) {
            le->setToolTip(src + " [" + datetime + "]");
        }

        // message needs more space

        row++;
        col = 0;

        QLabel *l = container->findChild<QLabel *>("l_msg");
        if(!l)
        {
            l = new QLabel("Message", container);
            l->setObjectName("l_msg");
            QFont f = l->font();
            f.setBold(true);
            l->setAlignment(Qt::AlignHCenter);
            l->setFont(f);
            glo->addWidget(l, row, col, 1, locolmax);
        }

        row++;
        col = 0;
        if(d.containsKey("msg"))
        {
            QLineEdit *te = container->findChild<QLineEdit *>("le_msg");
            if(!te)
            {
                te = new QLineEdit(container);
                te->setObjectName("le_msg");
                glo->addWidget(te, row, 0, 1, locolmax);
            }
            te->setText(QString::fromStdString(d["msg"].toString()));
            te->setToolTip(src + ": " + te->text());
        }



    }

    // write value, if available
    //    QLabel *lwv = new QLabel("w_value", monitorF);
    //    lwv->setAlignment(Qt::AlignRight);
    //    lwv->setObjectName("lwv");
    //    QLineEdit *lewv = new QLineEdit(monitorF);
    //    lewv->setObjectName("lewv");
    //    molo->addWidget(lwv, 2, 4, 1, 1);
    //    molo->addWidget(lewv, 2, 5, 1, 1);
    //    lwv->setDisabled(true); // will be shown if write value is available
    //    lewv->setDisabled(true); //



}

void CuInfoDialog::newLiveData(const CuData &data)
{
    if(data.containsKey("data_format_str"))
        sender()->disconnect(this, SLOT(newLiveData(const CuData&)));

    QString src = QString::fromStdString(data["src"].toString());
    std::string format = data["data_format_str"].toString();
    QWidget *plotw = NULL;
    QVBoxLayout *livelo = qobject_cast<QVBoxLayout *>(findChild<QGroupBox *>(src + "_live")->layout());
    if(format == "scalar")
    {
        QuTrendPlot *trplot = findChild<QuTrendPlot *>("trplot_" + src);
        if(!trplot)
        {
            // add the trend plot.
            // if the data is vector, the trend plot will be replaced by a spectrum plot
            if(d->cumbia)
                trplot = new QuTrendPlot(findChild<QFrame *>("liveF"), d->cumbia, *d->r_fac);
            else if(d->cu_pool)
                trplot = new QuTrendPlot(findChild<QFrame *>("liveF"), d->cu_pool, d->f_pool);
            if(trplot)
                plotw = trplot;
        }
    }
    else if(format == "vector")
    {
        QuSpectrumPlot *splot = findChild<QuSpectrumPlot *>("trplot_" + src);
        if(!splot)
        {
            if(d->cumbia)
                splot = new QuSpectrumPlot(findChild<QFrame *>("liveF"), d->cumbia, *d->r_fac);
            else if(d->cu_pool)
                splot = new QuSpectrumPlot(findChild<QFrame *>("liveF"),  d->cu_pool, d->f_pool);
            if(splot)
                plotw = splot;
        }
    }
    if(plotw)
    {
        plotw->setObjectName("trplot_" + src);
        plotw->setProperty("source", src);
        livelo->addWidget(plotw);
    }

}

HealthWidget::HealthWidget(QWidget *parent) : QLabel(parent) {
    setAlignment(Qt::AlignHCenter);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
}

void HealthWidget::paintEvent(QPaintEvent *e)
{
    QColor good = QColor(Qt::green);
    QColor err = QColor(Qt::red);
    float perc = property("health").toFloat();
    QPainter p(this);
    int x =qRound( width() * perc / 100.0);
    p.fillRect(0, 0, x, height(), good);
    p.fillRect(x + 1, 0, width() - x, height(), err);
    QLabel::paintEvent(e);
}

void HealthWidget::setData(int errcnt, int opcnt)
{
    float health = 100 - errcnt / (float) opcnt * 100.0;
    setText(QString("Health: %1%").arg(health, 0, 'f', 1));
    setProperty("health", health);
}
