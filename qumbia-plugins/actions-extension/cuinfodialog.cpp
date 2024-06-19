#include "cuinfodialog.h"
#include <cudata.h>
#include <QMetaObject>
#include <QMetaMethod>
#include <QtDebug>
#include <cumacros.h>
#include <egauge.h>
#include <cucontexti.h>
#include <culinkstats.h>
#include <cucontext.h>
#include <qutrendplot.h>
#include <QLineEdit>
#include <QLabel>
#include <QGridLayout>
#include <QPaintEvent>
#include <QScrollBar>
#include <QPainter>
#include <QMap>
#include <QMutableMapIterator>
#include <QApplication>
#include <cumacros.h>
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
#include <cumbiatango.h>
#include <cumbiahttp.h>
#include <quapps.h>
#include <algorithm>
#include <QRegularExpressionMatch>
#include <QMultiMap>
#include <quwatcher.h>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QPushButton>
#include <QGroupBox>
#include <QHeaderView>
#include <qustring.h>
#include <QTimer>
#include <cucontrolsutils.h>
#include <qupalette.h>

class CuInfoDEventFilterPrivate {
public:
    CuInfoDEventFilterPrivate(CuInfoDEventListener *_l) : l(_l), cur_obj(nullptr), next_obj(nullptr), cur_ctxi(nullptr),
        next_ctxi(nullptr), tmr(nullptr) {}
    CuInfoDEventListener *l;
    QObject *cur_obj, *next_obj;
    CuContextI *cur_ctxi, *next_ctxi;
    QTimer *tmr;
};

CuInfoDEventFilter::CuInfoDEventFilter(QObject *parent, CuInfoDEventListener *el) : QObject(parent) {
    d = new CuInfoDEventFilterPrivate(el);
}

CuInfoDEventFilter::~CuInfoDEventFilter() {
    printf("\e[1;31m~\e[0m deleting CuInfoDEventFilter %p\n", this);
    delete d;
}

bool CuInfoDEventFilter::eventFilter(QObject *obj, QEvent *event) {
    if(event->type() == QEvent::Enter) {
        CuContextI *ci = dynamic_cast<CuContextI *>(obj);
        if(ci && ci != d->cur_ctxi && obj != d->cur_obj) {
            d->next_ctxi = ci;
            d->next_obj = obj;
            if(!d->tmr) {
                d->tmr = new QTimer(this);
                d->tmr->setSingleShot(true);
                d->tmr->setInterval(1000);
                connect(d->tmr, SIGNAL(timeout()), this, SLOT(notify()));
            }
            d->tmr->start();
        }
        printf("\e[1;32menter event on object %p --> %s\e[0m\n", obj, qstoc(obj->objectName()));
    } else if (event->type() == QEvent::Leave) {
        d->next_obj = nullptr;
        d->next_ctxi = nullptr;
        printf("\e[1;35mleave event on object %p --> %s\e[0m\n", obj, qstoc(obj->objectName()));
    }
    return event->type() == QEvent::Enter || event->type() == QEvent::Leave;
}

void CuInfoDEventFilter::notify() {
    if(d->next_ctxi && d->next_obj) {
        d->cur_ctxi = d->next_ctxi;
        d->cur_obj = d->next_obj;
        if(d->l) d->l->onObjectChanged(d->cur_obj, d->cur_ctxi);
    }
}

class CuInfoDialogPrivate
{
public:
    CuInfoDialogPrivate() {}
    const CuContextI *ctxi;
    Cumbia *cumbia;
    CumbiaPool *cu_pool;
    CuControlsFactoryPool f_pool;
    const CuControlsReaderFactoryI *r_fac;
    int layout_col_cnt;
    QTreeWidget* tree;
    QMultiMap<QString, QString> categories;
};

CuInfoDialog::CuInfoDialog(QWidget *parent)
    : QDialog(parent) {
    d = new CuInfoDialogPrivate();
    d->r_fac = NULL; // pointer copied. object not cloned
    d->cu_pool = NULL;
    d->ctxi = nullptr;
    m_owner = nullptr;
    foreach(const QString& s, QStringList() << "value" << "w_value" << "write_value" << "date and time" << "err" << "msg")
        d->categories.insert("reader", s);
    QMap<QString, QString> apropmap = m_appPropMap();
    foreach(const QString& s, apropmap.keys())
        d->categories.insert("app", s);
    foreach(const QString& s, QStringList() << "operation count" << "error count" << "last error")
        d->categories.insert("stats", s);
    setAttribute(Qt::WA_DeleteOnClose, true);
}

CuInfoDialog::~CuInfoDialog() {
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

void CuInfoDialog::showAppDetails(bool show)
{
    //    if(show) {
    //        QGroupBox *appDetGb = new QGroupBox(this); // app details group box
    //        appDetGb->setObjectName("appDetailsGroupBox");
    //        int rowcnt = m_populateAppDetails(appDetGb);
    //        findChild<QGridLayout *>("mainGridLayout")->addWidget(appDetGb, mAppDetailsLayoutRow, 0, rowcnt, d->layout_col_cnt);
    //    }
    //    else {
    //        QGroupBox *appDetGb = findChild<QGroupBox *>("appDetailsGroupBox");
    //        if(appDetGb) {
    //            delete appDetGb;
    //            m_resizeToMinimumSizeHint();
    //        }
    //    }
}

QObject *root_obj(QObject *leaf) {
    QObject *root = leaf;
    while(root->parent())
        root = root->parent();
    return root != leaf ? root : nullptr;
}

void CuInfoDialog::exec(const CuData& in, const CuContextI *ctxi)
{
    d->ctxi = ctxi;
    d->layout_col_cnt = 8;
    resize(700, 720);
    int row = 0;
    m_owner = static_cast<QObject *>(in["sender"].toVoidP());
    QFont f = font();
    f.setBold(true);
    f.setPointSize(f.pointSize() + 1);
    QGridLayout * lo = new QGridLayout(this);
    d->tree = new QTreeWidget(this);
    d->tree->setColumnCount(3);
    d->tree->setHeaderHidden(true);
    d->tree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    lo->addWidget(d->tree, 0, 0, 10, 10);
    // update with live data
    onObjectChanged(m_owner, ctxi);
    //    m_resizeToMinimumSizeHint();
    show();

    CuInfoDEventFilter *efi = new CuInfoDEventFilter(this, this);
    QList<QWidget *> o = root_obj(m_owner)->findChildren<QWidget *>();
    foreach(QWidget *w, o) {
        printf("seeing if %p-->%s has source slot\n", w,qstoc(w->objectName()));
        if(w->metaObject()->indexOfProperty("source") > -1 || w->metaObject()->indexOfProperty("target") > -1) {
            w->installEventFilter(efi);
        }
    }

    QString formula, src = m_owner->property("source").toString();
    if(src.isEmpty())
        src = m_owner->property("target").toString();
    src = extractSource(src, formula);
    setWindowTitle(src + " stats");

    /// OCCHIO QUI
    ///
    ///
    return;
    ///
    ///
    ///



    if(d->ctxi->getContext() != nullptr) {
        CuLinkStats *lis = ctxi->getContext()->getLinkStats();

        QGridLayout *lo = new QGridLayout(this);
        lo->setObjectName("mainGridLayout");
        // use QLabel instead of group box title to use bold font
        QLabel *lobj = new QLabel("Monitored object", this);
        lobj->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
        QLineEdit *leName = new QLineEdit(m_owner->objectName(),this);
        leName->setReadOnly(true);

        QLabel *l_Type = new QLabel("Type", this);
        l_Type->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
        QLineEdit *leType = new QLineEdit(m_owner->metaObject()->className(), this);
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

        QList<CuControlsWriterA *> writers = ctxi->getContext()->writers();
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

        }

        QList<CuControlsReaderA *> readers = ctxi->getContext()->readers();
        // create a set of GroupBoxes that will contain monitor widgets
        foreach(CuControlsReaderA *r, readers)
        {
            QString formula, src;
            src = extractSource(r->source(), formula);
            QGroupBox *gb = new QGroupBox("", monitorF);
            gb->setObjectName(src + "_monitor");
            printf("\e[1;32mcreated GroupBox with name '%s'\e[0m\n", qstoc(gb->objectName()));
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



    } // d->ctxi->getContext() != nullptr

}

void CuInfoDialog::onMonitorUpdate(const CuData &da) {
    int scrollbarPos = d->tree->verticalScrollBar()->value();
    bool live = sender()->objectName() == "1Twatcher";
    QTextBrowser *tb = nullptr;
    if(da.has(TTT::Type, "property")) {  // has("type", "property")
        m_update_props(da, false);
    }
    if(!da.has(TTT::Type, "property") || live) {  // has("type", "property")
        m_update_value(da, live);
    }
    m_update_stats(QuString(da.s(TTT::Src)));  // da.s("src")
    d->tree->verticalScrollBar()->setValue(scrollbarPos); // restore scroll bar position
    ////////
    return;
    /////////
    ///
    ///
}

void CuInfoDialog::m_one_time_read(const QString &src, const CuContext *ctx) {
    CumbiaPool *p = ctx->cumbiaPool();
    Cumbia *c = ctx->cumbia();
    CuControlsReaderFactoryI* rfi = ctx->getReaderFactoryI();
    Qu1TWatcher *w = nullptr;
    if(p)
        w  = new Qu1TWatcher(this, ctx->cumbiaPool(), ctx->getControlsFactoryPool());
    else if(c && rfi) {
        w = new Qu1TWatcher(this, c, *rfi->clone());
    }
    if(w) {
        w->setObjectName("1Twatcher");
        connect(w, SIGNAL(newData(CuData)), this, SLOT(onMonitorUpdate(CuData)));
        w->setSource(src);
    }
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

QTreeWidgetItem *CuInfoDialog::m_readers_root() const {
    for(int i = 0; i < d->tree->topLevelItemCount(); i++)
        if(d->tree->topLevelItem(i)->text(0) == "readers")
            return d->tree->topLevelItem(i);
    return nullptr;
}

QTreeWidgetItem *CuInfoDialog::m_writers_root() const {
    for(int i = 0; i < d->tree->topLevelItemCount(); i++)
        if(d->tree->topLevelItem(i)->text(0) == "writers")
            return d->tree->topLevelItem(i);
    return nullptr;
}

QList<QTreeWidgetItem *> CuInfoDialog::m_reader_items() const {
    QTreeWidgetItem *it = m_readers_root();
    QList<QTreeWidgetItem *> ri;
    if(it == nullptr) {
        for(int i = 0; i < d->tree->topLevelItemCount(); i++)
            ri << d->tree->topLevelItem(i);
    } else {
        for(int i = 0; i < it->childCount(); i++)
            ri << it->child(i);
    }
    return ri;
}

QList<QTreeWidgetItem *> CuInfoDialog::m_writer_items() const {
    QTreeWidgetItem *it = m_writers_root();
    QList<QTreeWidgetItem *> wi;
    if(it == nullptr) {
        for(int i = 0; i < d->tree->topLevelItemCount(); i++)
            wi << d->tree->topLevelItem(i);
    } else {
        for(int i = 0; i < it->childCount(); i++)
            wi << it->child(i);
    }
    return wi;
}

QTreeWidgetItem *CuInfoDialog::m_find_reader(const QString &src) const {
    foreach(QTreeWidgetItem *it, m_reader_items())
        if(it->text(0) == src)
            return it;
    return nullptr;
}

QTreeWidgetItem *CuInfoDialog::m_find_writer(const QString &tgt) const {
    foreach(QTreeWidgetItem *it, m_writer_items())
        if(it->text(0) == tgt)
            return it;
    return nullptr;
}

QTreeWidgetItem *CuInfoDialog::m_get_reader_key(const QString &src, const QString &key) const {
    QTreeWidgetItem *it = m_find_reader(src);
    QTreeWidgetItem *category_it = it;
    if(it) {
        QString category = d->categories.key(key, "property"); // "value", "app", the others are "property"
        category_it = m_find_child(it, category);
        if(!category_it) {
            category_it = new QTreeWidgetItem(it, QStringList() << category);
            if(category == "reader") category_it->setExpanded(true);
        }
        // children of the src if "value" category or children of either "property" or "app"
        QTreeWidgetItem *key_it = m_find_child(category_it, key);
        if(key_it) {
            return key_it;
        }
        else if(category_it)  { // child key not found: add a new entry
            return new QTreeWidgetItem(category_it, QStringList() << key << "-");
        }
    } // src item exists
    return nullptr;
}

QTreeWidgetItem *CuInfoDialog::m_get_writer_key(const QString &tgt, const QString &key) const {
    QTreeWidgetItem *it = m_find_writer(tgt);
    if(it) {
        QString category = d->categories.key(key, "property"); // "value", "app", the others are "property"
        if(category == "app") {
            it = m_find_child(it, category);
            if(!it)
                it = new QTreeWidgetItem(it, QStringList() << category);
        }
        // children of the src if "value" category or children of either "property" or "app"
        QTreeWidgetItem *key_it = m_find_child(it, key);
        if(key_it)
            return key_it;
        else if(it) // child key not found: add a new entry
            return new QTreeWidgetItem(it, QStringList() << key << "-");
    } // src item exists
    return nullptr;
}

QTreeWidgetItem *CuInfoDialog::m_find_child(const QTreeWidgetItem *parent, const QString &key) const {
    for(int i = 0; parent != nullptr && i < parent->childCount(); i++)
        if(parent->child(i)->text(0) == key)
            return parent->child(i);
    return nullptr;
}

QTreeWidgetItem *CuInfoDialog::m_add_reader(const QString &src) {
    return !m_readers_root() ? new QTreeWidgetItem(d->tree, QStringList() << src) : new QTreeWidgetItem(m_readers_root(), QStringList() << src);
}

QTreeWidgetItem *CuInfoDialog::m_add_writer(const QString &tgt) {
    return !m_writers_root() ? new QTreeWidgetItem(d->tree, QStringList() << tgt) : new QTreeWidgetItem(m_writers_root(), QStringList() << tgt);
}

QTreeWidgetItem *CuInfoDialog::m_update_reader_key(const QString &src, const QString &key, const QString &value, int column) {
    QTreeWidgetItem *it = m_get_reader_key(src, key);
    //    if(it)
    it->setText(column, value);
    return it;
}

QTreeWidgetItem *CuInfoDialog::m_update_writer_key(const QString &tgt, const QString &key, const QString &value, int column) {
    QTreeWidgetItem *it = m_get_writer_key(tgt, key);
    //    if(it)
    it->setText(column, value);
    return it;
}

int CuInfoDialog::m_readers_count() const {
    return m_reader_items().size();
}

int CuInfoDialog::m_writers_count() const {
    return m_writer_items().size();
}

void CuInfoDialog::m_update_props(const CuData &da, bool writer) {
    QuString src(da.s(TTT::Src));  // da.s("src")
    if(da.has(TTT::Type, "property")) {  // has("type", "property")
        QStringList noprop_keys { "value", "w_value" , "write_value"};
        std::vector<std::string> dkeys = da.keys();
        std::sort(dkeys.begin(), dkeys.end());
        for(size_t i = 0; i < dkeys.size(); i++) {
            const QuString& s (dkeys[i]);
            if(!noprop_keys.contains(s)) {
                !writer ? m_update_reader_key(src, s, QuString(da[dkeys[i]]), 1) : m_update_writer_key(src, s, QuString(da[dkeys[i]]), 1);
                //                !writer ? m_update_reader_key(src, s, QuString(da[dkeys[i]]), 2) : m_update_writer_key(src, s, QuString(da[dkeys[i]]), 2);
            }
        }
    }
}

void CuInfoDialog::m_update_value(const CuData &da, bool live) {
    QuPalette pale;
    const QuString& s(da.s(TTT::Src));  // da.s("src")
    QTreeWidgetItem *it = m_find_reader(s);
    if(it) {
        double x = 0;
        QTreeWidgetItem * i = m_get_reader_key(s, "date and time");
        da[TTT::Time_ms].to<double>(x);  // da["timestamp_ms"]
        QString datetime = QDateTime::fromMSecsSinceEpoch(x).toString();
        QStringList valueKeys = QStringList() << "value" << "w_value" << "write_value" << "err";
        if(da.containsKey(TTT::Message)) valueKeys << "msg";  // da.containsKey("msg")
        else delete m_get_reader_key(s, "msg"); // remove "msg" item for source s
        i->setText(1, datetime);
        i->setText(2, (live ?  "value from a temporary reader" : "value from " + m_owner->objectName()));
        foreach(QString vk, valueKeys) {
            if(da.containsKey(vk.toStdString())) {
                i = m_get_reader_key(s, vk);
                i->setText(1, QuString(da[vk.toStdString()]));
                if(vk == "value") {
                    QFont f = i->font(1); f.setBold(true); i->setFont(1, f);
                    da.containsKey(TTT::QualityColor) && da.s(TTT::QualityColor) != "white" ? i->setForeground(1, pale.value(da.s(TTT::QualityColor).c_str()))  // da.containsKey("qc"), da.s("qc"), da.s("qc")
                                                                  : i->setForeground(1, QColor(Qt::black));

                } else if(vk == "err") {
                    i->setForeground(1, da.B(TTT::Err) ? QColor("red") : QColor("black"));  // da.B("err")
                }
            }
        }
    }
}

void CuInfoDialog::m_update_stats(const QString& src) {
    if(!src.isEmpty()) {
        CuLinkStats *lis = d->ctxi->getContext()->getLinkStats();
        m_update_reader_key(src, "operation count", QString::number(lis->opCnt()), 1);
        m_update_reader_key(src, "error count", QString::number(lis->errorCnt()), 1);
        m_update_reader_key(src, "last error", lis->last_error_msg.c_str(), 1);
        //    HealthWidget *healthw  = findChild<HealthWidget *>();
        //    healthw->setData(lis->errorCnt(), lis->opCnt());
    }
}

void CuInfoDialog::onObjectChanged(QObject *obj, const CuContextI *ctxi) {
    pretty_pri("obj %s class %s", qstoc(obj->objectName()), obj->metaObject()->className());
    if(ctxi && ctxi->getContext() && ctxi->getContext()->getReader())
        printf("source %s\n", qstoc(ctxi->getContext() ->getReader()->source()));
    else if(ctxi && ctxi->getContext() && ctxi->getContext() ->getWriter()) {
        printf("source %s\n", qstoc(ctxi->getContext() ->getWriter()->target()));
    }

    disconnect(m_owner, SIGNAL(newData(CuData)), this, SLOT(onMonitorUpdate(CuData)));
    m_owner = obj;
    QFont f = font();
    f.setBold(true);
    f.setPointSize(f.pointSize() + 1);
    // update with live data
    connect(m_owner, SIGNAL(newData(const CuData&)), this, SLOT(onMonitorUpdate(const CuData&)));
    d->ctxi = ctxi;

    if(!ctxi || !ctxi->getContext())
        return;
    CuContext *ctx = ctxi->getContext();
    int i;
    QStringList vsrcs, vtgts;
    for(CuControlsReaderA *r : ctx->readers())
        vsrcs.push_back(r->source());
    for(CuControlsWriterA *w : ctx->writers())
        vtgts.push_back(w->target());

    // remove unused items
    QList<QTreeWidgetItem *> its = m_reader_items();
    foreach(QTreeWidgetItem *it, its)
        if(!vsrcs.contains(it->text(0)))
            delete it;
    its = m_writer_items();
    foreach(QTreeWidgetItem *it, its)
        if(!vtgts.contains(it->text(0)))
            delete it;

    // create a "readers" and a "writers" item only if there are readers *and* writers
    // in the same context
    //
    if(ctx->readers().size() > 0 && ctx->writers().size() > 0) {
        QTreeWidgetItem *root = m_readers_root();
        if(!root)
            root = new QTreeWidgetItem(d->tree, QStringList() << "readers");
        root = m_writers_root();
        if(!root)
            root = new QTreeWidgetItem(d->tree, QStringList() << "writers");
        if(root)
            root->setExpanded(true);
    }
    for(CuControlsReaderA * r : ctx->readers()) {
        QTreeWidgetItem *it = m_find_reader(r->source());
        if(!it) {
            it = m_add_reader(r->source());
            it->setExpanded(true);
            // starts one watcher for first reading.
            m_one_time_read(r->source(), ctx);
        }
    }
    for(CuControlsWriterA * w : ctx->writers()) {
        QTreeWidgetItem *it = m_find_writer(w->target());
        if(!it) {
            it = m_add_writer(w->target());
            it->setExpanded(true);
            m_update_props(w->getConfiguration(), true); // true: from a writer
        }
    }
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
