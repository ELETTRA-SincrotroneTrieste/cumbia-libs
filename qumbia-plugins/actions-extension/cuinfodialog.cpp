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
#include <QLineEdit>
#include <QLabel>
#include <QTextBrowser>
#include <QComboBox>
#include <QCheckBox>
#include <QScrollArea>
#include <QGridLayout>
#include <QFrame>
#include <QPaintEvent>
#include <QScrollBar>
#include <QPainter>
#include <QMap>
#include <QMutableMapIterator>
#include <QPushButton>
#include <QApplication>
#include <quledbase.h>
#include <cumacros.h>
#include <qulabel.h>
#include <quspectrumplot.h>
#include <cumbia.h>
#include <cumbiapool.h>
#include <cucontrolsfactories_i.h>
#include <cucontrolsfactorypool.h>
#include <cucontrolsreader_abs.h>
#include <cucontrolswriter_abs.h>
#include <cucontext.h>
#include <cuformulaplugininterface.h>
#include <cupluginloader.h>
#include <QPluginLoader>
#include <QRadioButton>
#include <cumbiatango.h>
#include <cumbiahttp.h>

class CuInfoDialogPrivate
{
public:
    CuInfoDialogPrivate() {}
    const CuContext *ctx;
    Cumbia *cumbia;
    CumbiaPool *cu_pool;
    CuControlsFactoryPool f_pool;
    const CuControlsReaderFactoryI *r_fac;
    int layout_col_cnt;
};

CuInfoDialog::CuInfoDialog(QWidget *parent)
    : QDialog(parent)
{
    d = new CuInfoDialogPrivate();
    d->r_fac = NULL; // pointer copied. object not cloned
    d->cu_pool = NULL;
    d->ctx = nullptr;
    setAttribute(Qt::WA_DeleteOnClose, true);
}

CuInfoDialog::~CuInfoDialog()
{
    // do not delete d->rfac because the reference has been
    // copied, not cloned
    delete d;
}

/**
 * @brief CuInfoDialog::extractSources if source represents a formula, this method extracts the
 *        source names only, comma separated if more than one. The result must be formatted in a
 *        way that is compatible with the "src" CuData value that will be provided by the formula
 *        reader, for example: *test/device/1/double_scalar,test/device/2/double_scalar*
 * @param expression the source expression as returned by the *source* property of the reader
 * @param formula will contain the formula, if any
 * @return a list of strings with the sources detected in expr.
 *
 * Please read the *cuformula* plugin documentation for further details.
 * If expression is a formula, the list of sources involved are detected within the first
 * *{}* parenthesis group.
 *
 * \par Example
 * If expression is "{$1/double_scalar,$2/double_scalar}  function(a,b) {  return a-b;}"
 * the method will return a QStringList ("$1/double_scalar","$2/double_scalar") and formula
 * will contain "function(a,b) {  return a-b;}"
 */
QStringList CuInfoDialog::extractSources(const QString &expression, QString& formula)
{
    QStringList srcs;
    CuPluginLoader plulo;
    QString plupath = plulo.getPluginAbsoluteFilePath(CUMBIA_QTCONTROLS_PLUGIN_DIR, "cuformula-plugin.so");
    QPluginLoader pluginLoader(plupath);
    QObject *plugin = pluginLoader.instance();
    bool error = !plugin;
    if (plugin){
        CuFormulaPluginI *fplu = qobject_cast<CuFormulaPluginI *>(plugin);
        if(!fplu) {
            perr("Failed to load formula plugin");
            error = true;
        }
        else {
            CuFormulaParserI *fparser = fplu->getFormulaParserInstance();
            fparser->parse(expression);
            error = fparser->error();
            if(!error) {
                if(!fparser->name().isEmpty())
                    srcs << fparser->name();
                else {
                    std::vector<std::string> vsrcs = fparser->sources();
                    foreach(std::string s, vsrcs)
                        srcs << QString::fromStdString(s);
                }
            }
        }
    }
    if(error) { // no formula or failed to load plugin, hopefully it's a valid source
        srcs << expression;
    }
    return srcs;
}

/**
 * @brief CuInfoDialog::extractSource provides the list of strings returned by extractSources joined by commas.
 * @param expression the expression obtained by the reader's source property (may be a simple source or a complex
 *        formula expression)
 * @param formula will contain the formula, if any
 * @return a comma separated list of strings that are the names of the sources detected within {}
 *
 * Please read the *cuformula* plugin documentation for further details.
 *
 * If expression is a formula, the list of sources involved are detected within the first
 * *{}* parenthesis group.
 *
 * \par Example
 * If expression is "{$1/double_scalar,$2/double_scalar}  function(a,b) {  return a-b;}"
 * the method will return a QString "$1/double_scalar,$2/double_scalar" and formula
 * will contain "function(a,b) {  return a-b;}"
 */
QString CuInfoDialog::extractSource(const QString &expression, QString &formula)
{
    return extractSources(expression, formula).join(",");
}

void CuInfoDialog::liveReadCbToggled(bool start)
{
    if(!start) {
        // liveFrame becomes a child of scrollArea
        // it will be deleted alongside all liveFrame's children
        //
        if(findChild<QScrollArea *>("liveScrollArea"))
            delete findChild<QScrollArea *>("liveScrollArea");
        m_resizeToMinimumSizeHint();
    }
    else {
        QList<CuControlsReaderA *> readers = d->ctx->readers();
        QGridLayout *mainLo = findChild<QGridLayout *>("mainGridLayout");
        int row = mainLo->rowCount();
        QScrollArea *scrollArea = new QScrollArea(this);
        scrollArea->setObjectName("liveScrollArea");
        scrollArea->setWidgetResizable(true);
        QFrame *liveF = new QFrame(this);
        liveF->setObjectName("liveF");
        // add the live scroll area with the labels+plots frame inside
        scrollArea->setWidget(liveF);
        scrollArea->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding));
        mainLo->addWidget(scrollArea, row, 0, 10, d->layout_col_cnt); // below monitorF
        QGridLayout *lilo = new QGridLayout(liveF);
        QFont f = font();
        f.setPointSize(f.pointSize() + 1);
        f.setBold(true);
        foreach(CuControlsReaderA *r, readers)
        {
            QString formula, src = extractSource(r->source(), formula);
            QGroupBox *gb = new QGroupBox(src, liveF);
            gb->setFont(f);
            lilo->addWidget(gb, row, 0, 1, d->layout_col_cnt);
            gb->setObjectName(src + "_live");
            QVBoxLayout * gblilo = new QVBoxLayout(gb);
            QuLabel *llive = NULL;
            if(d->ctx->cumbia() && d->ctx->getReaderFactoryI())
                llive = new QuLabel(liveF, d->ctx->cumbia(), *d->ctx->getReaderFactoryI());
            else if(d->ctx->cumbiaPool())
                llive = new QuLabel(liveF, d->ctx->cumbiaPool(), d->ctx->getControlsFactoryPool());
            if(llive)
            {
                connect(llive, SIGNAL(newData(CuData)), this, SLOT(newLiveData(CuData)));
                llive->setSource(r->source());
                llive->setMaximumLength(80);
                gblilo->addWidget(llive);
                QTabWidget *liveTabW = new QTabWidget(liveF);
                liveTabW->setObjectName(src + "liveTabW");
                liveTabW->addTab(new QWidget(liveF), "Introspect");
                gblilo->addWidget(liveTabW);
                // add a layout to the tab widget's first widget
                new QGridLayout(liveTabW->widget(0));
            }
            row++;
        }
    }
}

void CuInfoDialog::showAppDetails(bool show)
{
    if(show) {
        QGroupBox *appDetGb = new QGroupBox(this); // app details group box
        appDetGb->setObjectName("appDetailsGroupBox");
        int rowcnt = m_populateAppDetails(appDetGb);
        findChild<QGridLayout *>("mainGridLayout")->addWidget(appDetGb, mAppDetailsLayoutRow, 0, rowcnt, d->layout_col_cnt);
    }
    else {
        QGroupBox *appDetGb = findChild<QGroupBox *>("appDetailsGroupBox");
        if(appDetGb) {
            delete appDetGb;
            m_resizeToMinimumSizeHint();
        }
    }
}

QObject *root_obj(QObject *leaf) {
    QObject *root = leaf;
    while(root->parent())
        root = root->parent();
    return root != leaf ? root : nullptr;
}

void CuInfoDialog::exec(const CuData& in, const CuContext* ctx)
{
    d->ctx = ctx;
    d->layout_col_cnt = 8;
    resize(700, 720);
    int row = 0;
    QObject *sender = static_cast<QObject *>(in["sender"].toVoidP());
    QFont f = font();
    f.setBold(true);
    f.setPointSize(f.pointSize() + 1);
    // update with live data
    connect(sender, SIGNAL(newData(const CuData&)), this, SLOT(onMonitorUpdate(const CuData&)));

    QString formula, src = sender->property("source").toString();
    if(src.isEmpty())
        src = sender->property("target").toString();
    src = extractSource(src, formula);
    setWindowTitle(src + " stats");
    CuLinkStats *lis = ctx->getLinkStats();

    QGridLayout *lo = new QGridLayout(this);
    lo->setObjectName("mainGridLayout");
    // use QLabel instead of group box title to use bold font
    QLabel *lobj = new QLabel("Monitored object", this);
    lobj->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    QLineEdit *leName = new QLineEdit(sender->objectName(),this);
    leName->setReadOnly(true);

    QLabel *l_Type = new QLabel("Type", this);
    l_Type->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    QLineEdit *leType = new QLineEdit(sender->metaObject()->className(), this);
    leType->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

    leType->setFont(f);
    leName->setFont(f);

    //    QGroupBox *appDetGb = new QGroupBox(this); // app details group box
    //    appDetGb->setObjectName("appDetailsGroupBox");

    // button to toggle app details visibility
    QPushButton *pbShowAppDetails = new QPushButton("More...", this);
    pbShowAppDetails->setCheckable(true);
    pbShowAppDetails->setChecked(false);
    connect(pbShowAppDetails, SIGNAL(toggled(bool)), this, SLOT(showAppDetails(bool)));

    foreach(QLabel *l, QList<QLabel*> () << lobj << l_Type )
        l->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);


    lo->addWidget(lobj, row, 0, 1, 1);
    lo->addWidget(leName, row, 1, 1, 3);
    lo->addWidget(l_Type, row, 4, 1, 1);
    lo->addWidget(leType, row, 5, 1, 2);
    lo->addWidget(pbShowAppDetails, row, 7, 1, 1);

    row++;
    mAppDetailsLayoutRow = row;
    row += m_appPropMap().size() / 2;

    // operation count
    QLabel *lopcnt = new QLabel("Operation count:", this);
    QLineEdit *leopcnt = new QLineEdit(this);
    leopcnt->setObjectName("leopcnt");
    leopcnt->setReadOnly(true);
    leopcnt->setText(QString::number(lis->opCnt()));
    lo->addWidget(lopcnt, row, 0, 1, 1);
    lo->addWidget(leopcnt, row, 1, 1, 3);

    // error count
    QLabel *lerrcnt = new QLabel("Error count:", this);
    lerrcnt->setAlignment(Qt::AlignRight);
    QLineEdit *leerrcnt = new QLineEdit(this);
    leerrcnt->setObjectName("leerrcnt");
    leerrcnt->setReadOnly(true);
    leerrcnt->setText(QString::number(lis->errorCnt()));
    lo->addWidget(lerrcnt, row, 4, 1, 1);
    lo->addWidget(leerrcnt, row, 5, 1, 3);

    row++;

    if(lis->errorCnt() >= 0)
    {
        QLabel *l_lasterr = new QLabel("Last err", this);
        l_lasterr->setAlignment(Qt::AlignRight);
        lo->addWidget(l_lasterr, row, 0, 1, 1);
        QLineEdit *te_lasterr = new QLineEdit(this);
        te_lasterr->setReadOnly(true);
        te_lasterr->setObjectName("te_lasterr");
        lo->addWidget(te_lasterr, row, 1, 1, d->layout_col_cnt - 3);
        te_lasterr->setText(lis->last_error_msg.c_str());
        te_lasterr->setToolTip(te_lasterr->text());
    }

    row += 1;

    // Health
    HealthWidget *healthWidget = new HealthWidget(this);
    healthWidget->setData(lis->errorCnt(), lis->opCnt());
    lo->addWidget(healthWidget, row, 0, 1, d->layout_col_cnt);
    row++;

    int monrow = 0;
    QFrame *monitorF = new QFrame(this);
    monitorF->setObjectName("monitorF");
    QGridLayout *molo = new QGridLayout(monitorF);
    molo->setObjectName(monitorF->objectName() + "_layout");

    QList<CuControlsWriterA *> writers = ctx->writers();
    foreach (CuControlsWriterA* w, writers) {
        QGroupBox *gb = new QGroupBox("", monitorF);
        gb->setObjectName(w->target() + "_write_monitor");
        QVBoxLayout* gblo = new QVBoxLayout(gb);
        gblo->setObjectName(gb->objectName() + "_gridLayout");
        molo->addWidget(gb, monrow, 0, 1, d->layout_col_cnt);
        monrow++;
        // label with bold font indicating the source
        QLabel *slabel = new QLabel(gb);
        slabel->setObjectName("l_source_name");
        slabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        slabel->setStyleSheet("QLabel { background-color : white; color:DodgerBlue; margin:5px; padding:5px; "
                              " border: 1px solid DodgerBlue; border-radius:5px;  }");
        gblo->addWidget(slabel);
        // bold font on target name label
        QFont fo = slabel->font();
        fo.setBold(true);
        slabel->setFont(fo);
        slabel->setText(w->target());


        CuData confd = w->getConfiguration();
        if(!confd.isEmpty()) {
            QTextBrowser *teconf = new QTextBrowser(gb);
            teconf->setObjectName("tb_properties");
            teconf->setReadOnly(true);
            QString html = m_makeHtml(confd, "Properties");
            teconf->setHtml(html);
            gblo->addWidget(teconf);
        }
    }

    QList<CuControlsReaderA *> readers = ctx->readers();
    // create a set of GroupBoxes that will contain monitor widgets
    foreach(CuControlsReaderA *r, readers)
    {
        QString formula, src;
        src = extractSource(r->source(), formula);
        QGroupBox *gb = new QGroupBox("", monitorF);
        gb->setObjectName(src + "_monitor");
        QVBoxLayout* gblo = new QVBoxLayout(gb);
        gblo->setObjectName(gb->objectName() + "_gridLayout");
        molo->addWidget(gb, monrow, 0, 1, d->layout_col_cnt);
        monrow++;
        // label with bold font indicating the source
        QLabel *slabel = new QLabel(gb);
        slabel->setObjectName("l_source_name");
        slabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        slabel->setStyleSheet("QLabel { background-color : white; color:DodgerBlue; margin:5px; padding:5px; "
                              " border: 1px solid DodgerBlue; border-radius:5px;  }");
        QFont fo = slabel->font();
        fo.setBold(true);
        slabel->setFont(fo);
        slabel->setText(r->source());
        gblo->addWidget(slabel);

        // place a label saying "wait for next refresh"
        QLabel *label = new QLabel(gb);
        label->setObjectName("l_waitupdate");
        label->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        label->setText("waiting for next update...");
        gblo->addWidget(label);
    }
    // add the group boxes to the layout
    QSizePolicy monSp;
    // no readers: monitorF can take up more vertical space. There's readers: size policy needs
    // to be fixed otherwise no space for live frame
    readers.size() > 0 ? monSp = QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed) :
            monSp = QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    monitorF->setSizePolicy(monSp);
    lo->addWidget(monitorF, row, 0, 1, d->layout_col_cnt); // below QLabel wit src
    row++;

    // live stuff
    // Live Frame
    if(readers.size() > 0) {

        QCheckBox *cb = new QCheckBox("Start a live reader", this);
        cb->setFont(f);
        cb->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        connect(cb, SIGNAL(toggled(bool)), this, SLOT(liveReadCbToggled(bool)));
        lo->addWidget(cb, row, 0, 1, d->layout_col_cnt);
        row++;
    }


    //
    // engine hot switch (since 1.5.0)
    //
    QObject *root = root_obj(sender);
    printf("\e[1;32mCuInfoDialog::exec: root is %s (%s) from leaf %s (%s)\e[0m\n", root != nullptr ? qstoc(root->objectName())
                                                                                                   : "nullptr", root != nullptr ? root->metaObject()->className() : "-", qstoc(sender->objectName()), sender->metaObject()->className());
    // get current engine in use
    Cumbia *c = nullptr;
    if(ctx) {
        if(ctx->getReader())
            c = ctx->getReader()->getCumbia();
        else {
            if(ctx->getWriter())
                c = ctx->getWriter()->getCumbia();
        }
        if(c) {
            QGroupBox *gb = new QGroupBox("Engine hot switch", this);
            QHBoxLayout *hlo = new QHBoxLayout(gb);
#ifdef QUMBIA_TANGO_CONTROLS_VERSION
            QRadioButton *rbn = new QRadioButton("native", gb);
            rbn->setObjectName("rbn");
#endif
#ifdef CUMBIA_HTTP_VERSION
            QRadioButton *rbh = new QRadioButton("http", gb);
            rbh->setObjectName("rbh");
#endif
            if(c->getType() == CumbiaTango::CumbiaTangoType && gb->findChild<QRadioButton *>("rbn"))
                gb->findChild<QRadioButton *>("rbn")->setChecked(true);
            else if(c->getType() == CumbiaHttp::CumbiaHTTPType && gb->findChild<QRadioButton *>("rbh"))
                gb->findChild<QRadioButton *>("rbh")->setChecked(true);

            // add radios to layout
            if(gb->findChild<QRadioButton *>("rbn"))
                hlo->addWidget(gb->findChild<QRadioButton *>("rbn"));
            if(gb->findChild<QRadioButton *>("rbh"))
                hlo->addWidget(gb->findChild<QRadioButton *>("rbh"));

            lo->addWidget(gb, ++row, 0, 2, lo->columnCount());
        }

    }

    m_resizeToMinimumSizeHint();
    show();
}

void CuInfoDialog::onMonitorUpdate(const CuData &da)
{
    double x;
    CuLinkStats *lis = d->ctx->getLinkStats();
    findChild<QLineEdit *>("leopcnt")->setText(QString::number(lis->opCnt()));
    findChild<QLineEdit *>("leerrcnt")->setText(QString::number(lis->errorCnt()));
    findChild<QLineEdit *>("te_lasterr")->setText(lis->last_error_msg.c_str());
    HealthWidget *healthw  = findChild<HealthWidget *>();
    healthw->setData(lis->errorCnt(), lis->opCnt());

    QString src = QString(da["src"].toString().c_str());
    QGroupBox *container = findChild<QGroupBox *>(src + "_monitor");
    if(!container) // try if it is write
        container = findChild<QGroupBox *>(src + "_write_monitor");
    if(container) {
        QVBoxLayout *glo = qobject_cast<QVBoxLayout *>(container->layout());
        da["timestamp_ms"].to<double>(x);
        if(container)
        {
            QLabel* update_wait_l = container->findChild<QLabel *>("l_waitupdate");
            if(update_wait_l)
                delete update_wait_l;

            QTextBrowser *te = container->findChild<QTextBrowser *>("tb_monitor_update");
            if(!te) {
                te = new QTextBrowser(container);
                te->setReadOnly(true);
                te->setObjectName("tb_monitor_update");
                glo->addWidget(te);
            }
            int scrollbarPos = te->verticalScrollBar()->value();
            te->setHtml(m_makeHtml(da, "DATA"));
            te->verticalScrollBar()->setValue(scrollbarPos);
        }
    }
    else {
        perr("CuInfoDialog::onMonitorUpdate: either expected container %s_monitor or %s_write_monitor not found",
             qstoc(src),qstoc(src));
    }
}

QString CuInfoDialog::m_makeHtml(const CuData& da, const QString& heading) {
    double x;
    da["timestamp_ms"].to<double>(x);
    QString datetime = QDateTime::fromMSecsSinceEpoch(x).toString();

    QString html = "<html>\n";
    html += "<head>\n";
    html += "<style>\n";
    html += QString("table, th, td {  \
                    border-style: groove; \
            border-color: DodgerBlue; \
    border-width: 1px; \
    border-collapse: collapse; \
margin:0.3em; \
} \
\
th { \
    background-color: #4CAF50; \
color: white; \
} \
\
div { padding:0.1em; margin: 0.2em; } \
table { margin:3.5em; padding:2.4em; border-collapse: collapse; \
    cellspacing:0.2em \
    cellpadding:0.2em \
    width:80%; \
      } \
td { \
    width=50%; \
} \
div { width=80%; } \
\n");
 html += "</style>\n";
 html += "</head>\n";
 html += "<body>\n";
 html += "<h4 align=\"center\">" + heading + "</h4>\n";
 html += "<div id=\"tablesdiv\">\n";


 QString values_s;
 QStringList valueKeys = QStringList() << "value" << "w_value" << "write_value";
 foreach(QString vk, valueKeys) {
     if(da.containsKey(vk.toStdString()))
         values_s += "<tr><td>" + vk + "</td><td>" +
                 QString::fromStdString(da[vk.toStdString()].toString()) + "</td></tr>";
 }

 if(x > 0 || values_s.length() > 0) { // valid date and time or at least one of valueKeys found

     html += "<table>\n<tr><th colspan=\"2\"><cite>value</cite></th></tr>";
     if(x > 0)
         html += "<tr><td>date/time</td><td>" + datetime + "</td></tr>";
     html += values_s;
     html += "</table>\n\n\n";
 }


 QStringList priorityKeys = QStringList() << "src" << "device" << "point" <<
                                             "mode" << "err" << "msg" << "period" <<
                                             "dfs";

 // 1. information table
 html += "<table>\n<tr><th colspan=\"2\"><cite>information</cite></th></tr>";
 foreach(QString pk, priorityKeys) {
     if(da.containsKey(pk.toStdString()))
         html += "<tr><td>" + pk + "</td><td>" +
                 QString::fromStdString(da[pk.toStdString()].toString()) + "</td></tr>";
 }
 html += "</table>\n\n";

 // 2. advanced table
 const std::vector<std::string> &dkeys = da.keys();
 html += "<table>\n<tr><th colspan=\"2\"><cite>advanced</cite></th></tr>";
 for(size_t i = 0; i < dkeys.size(); i++) {
     const std::string& s = dkeys[i];
     if(!priorityKeys.contains(QString::fromStdString(s)))
         html += "<tr><td>" + QString::fromStdString(s) + "</td><td>" +
                 QString::fromStdString(da[dkeys[i]].toString()) + "</td></tr>";
 }
 html += "</table>\n\n";
 html += "</div> <!-- tablesdiv -->\n\n";
 html += "</body>\n</html>\n";

 //    printf("OUT\n\n%s\n", qstoc(html.remove("\n")));
 return html;
 }

 QMap<QString, QString> CuInfoDialog::m_appPropMap() const
 {
     QMap<QString, QString> app_p; // app properties
     app_p["App name"] = qApp->applicationName();
     app_p["Version"] = qApp->applicationVersion();
     app_p["Platform"] = qApp->platformName();
     app_p["Author"] = qApp->property("author").toString();
     app_p["e-mail"] = qApp->property("mail").toString();
     app_p["Phone"]  = qApp->property("phone").toString();
     app_p["Office"]  = qApp->property("office").toString();
     app_p["Hardware referent"]  = qApp->property("hwReferent").toString();
     app_p["Organization name"] = qApp->organizationName();
     app_p["PID"] =  QString::number(qApp->applicationPid());
     app_p["App"] = qApp->applicationFilePath();
     QMutableMapIterator<QString, QString> i(app_p);
     while(i.hasNext()) {
         i.next();
         if(i.value().isEmpty() || i.value().contains("$"))
             i.remove();
     }
     return app_p;
 }

 int CuInfoDialog::m_populateAppDetails(QWidget *container)
 {
     QGridLayout *lo = new QGridLayout(container);
     QStringList orderedKeys = QStringList() << "App name" <<"Version"  <<"Platform" <<"Author" << "e-mail" << "Phone"
                                             <<"Office" << "Hardware referent" << "PID" << "App";
     QMap<QString, QString> app_p = m_appPropMap(); // app properties
     int r = 0;
     int c = 0;
     foreach(QString k, orderedKeys) {
         if(app_p[k].length() > 0 && !app_p[k].contains("$")) {
             if(c >= 8)
                 c = 0;
             QLabel *l = new QLabel(k, this);
             l->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
             QLineEdit *ln = new QLineEdit(app_p[k], this);
             ln->setReadOnly(true);
             lo->addWidget(l, r / 2, c, 1, 1);
             lo->addWidget(ln, r / 2, c + 1, 1, 3);
             c += 4;
             r++;
         }
     }
     return r / 4;
 }

 void CuInfoDialog::m_resizeToMinimumSizeHint() {
     resize(qRound(minimumSizeHint().width() * 1.5), minimumSizeHint().height());
 }

 void CuInfoDialog::newLiveData(const CuData &data)
 {
     QString src = QString::fromStdString(data["src"].toString());
     std::string format = data["dfs"].toString();
     QWidget *plotw = NULL;
     QTabWidget *liveTabW = findChild<QTabWidget *>(src + "liveTabW");
     QuTrendPlot *trplot = findChild<QuTrendPlot *>("trplot_" + src);
     QuSpectrumPlot *splot = findChild<QuSpectrumPlot *>("trplot_" + src);
     if(liveTabW) {
         if(!trplot && !splot) {
             Cumbia *cumbia = d->ctx->cumbia();
             CuControlsReaderFactoryI *rfac = d->ctx->getReaderFactoryI();
             CumbiaPool *cu_pool = d->ctx->cumbiaPool();
             CuControlsFactoryPool fpool = d->ctx->getControlsFactoryPool();

             if(format == "scalar")
             {
                 if(!trplot)
                 {
                     // add the trend plot.
                     // if the data is vector, the trend plot will be replaced by a spectrum plot
                     if(cumbia && rfac)
                         trplot = new QuTrendPlot(findChild<QFrame *>("liveF"), cumbia, *rfac);
                     else if(cu_pool)
                         trplot = new QuTrendPlot(findChild<QFrame *>("liveF"), cu_pool, fpool);
                     if(trplot)
                         plotw = trplot;
                 }
             }
             else if(format == "vector")
             {

                 if(!splot)
                 {
                     if(cumbia)
                         splot = new QuSpectrumPlot(findChild<QFrame *>("liveF"), cumbia, *rfac);
                     else if(d->cu_pool)
                         splot = new QuSpectrumPlot(findChild<QFrame *>("liveF"),  cu_pool, fpool);
                     if(splot)
                         plotw = splot;
                 }
             }
             if(plotw)
             {
                 plotw->setObjectName("trplot_" + src);
                 QStringList srcs;
                 if(data.containsKey("formula") && data.containsKey("srcs")) {
                     std::string formula_src;
                     std::string srclist;
                     std::vector<std::string> vsrcs = data["srcs"].toStringVector();
                     std::string fname = data["src"].toString(); // formula name [in square brackets]
                     for(size_t i = 0; i < vsrcs.size(); i++) {
                         i < vsrcs.size() - 1 ? srclist += vsrcs[i] + "," : srclist += vsrcs[i];
                         srcs << QString::fromStdString(vsrcs[i]);
                     }
                     formula_src = "formula://";
                     if(fname.length() > 0)
                         formula_src += "[" + fname + "]";
                     formula_src += "{" + srclist + "}" + data["formula"].toString();
                     srcs << QString::fromStdString(formula_src);
                 }
                 else {
                     srcs << src;
                 }

                 qDebug() << __PRETTY_FUNCTION__ << "setting sources srcs " << srcs;
                 plotw->setProperty("sources", srcs);
                 liveTabW->addTab(plotw, "Plot");
             }
         }

         if(data["type"].toString() == "property") {
             QString html = m_makeHtml(data, "properties of \"" + src + "\"");
             QTextBrowser *tbp = findChild<QFrame *>("liveF")->findChild<QTextBrowser *>("tb_live_properties");
             if(!tbp) {
                 tbp = new QTextBrowser(findChild<QFrame *>("liveF"));
                 tbp->setReadOnly(true);
                 tbp->setObjectName("tb_live_properties");
                 QGridLayout *glo = qobject_cast<QGridLayout*>(liveTabW->widget(0)->layout());
                 glo->addWidget(tbp, 0, 0, 4, 4);
             }
             tbp->setHtml(html);
         }
         else {
             QString html = m_makeHtml(data, "data from \"" + src + "\"");
             QTextBrowser *tbpdata = findChild<QFrame *>("liveF")->findChild<QTextBrowser *>("tb_live_data");
             if(!tbpdata) {
                 tbpdata = new QTextBrowser(findChild<QFrame *>("liveF"));
                 tbpdata->setReadOnly(true);
                 tbpdata->setObjectName("tb_live_data");
                 QGridLayout *glo = qobject_cast<QGridLayout*>(liveTabW->widget(0)->layout());
                 glo->addWidget(tbpdata, 0, 4, 4, 4);
             }
             tbpdata->setHtml(html);
         }
     }
     else {
         perr("CuInfoDialog::newLiveData: expected container %s_live not found", qstoc(src));
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
