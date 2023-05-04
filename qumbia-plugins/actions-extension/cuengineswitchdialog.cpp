#include "cuengineswitchdialog.h"
#include "cuengineaccessor.h"
#include <QMessageBox>
#include <QLabel>
#include <QRadioButton>
#include <qgridlayout.h>
#include <quapps.h>
#include <cucontexti.h>
#include <cucontext.h>

class CuEngineSwitchDialogPrivate {
public:
    QObject *owner;
    const CuContextI *ctxi;
};

CuEngineSwitchDialog::CuEngineSwitchDialog(QWidget *parent, const CuContextI *ctxi) : QDialog(parent)
{
    d = new CuEngineSwitchDialogPrivate;
    d->ctxi = ctxi;
}

CuEngineSwitchDialog::~CuEngineSwitchDialog()
{
    delete d;
}

void CuEngineSwitchDialog::m_resizeToMinimumSizeHint() {
    resize(qRound(minimumSizeHint().width() * 1.5), minimumSizeHint().height());
}

void CuEngineSwitchDialog::onEngineChanged(const QStringList &from, const QStringList &to) {
    QString txt = QString("swapped engine <i>cumbia-%1</i> with <i>cumbia-%2</i>").arg(from.join(',')).arg(to.join(','));
    QLabel *l = findChild<QLabel *>("lEngChanged");
    QGridLayout *lo = findChild<QGridLayout *>("main_lo");
    if(!l) {
        l = new QLabel("", this);
        l->setObjectName("lEngChanged");
        lo->addWidget(l, lo->rowCount(), 0, 1, lo->columnCount());
    }
    l->setText(txt);
}

void CuEngineSwitchDialog::exec(const CuData &in, const CuContextI *ctxi)
{
    d->owner = static_cast<QObject *>(in["sender"].toVoidP());
    printf("ctxi %p  d->ctxi %p\n", ctxi, d->ctxi);
    //
    // engine hot switch (since 1.5.0)
    //
    QObject *root = root_obj(d->owner);
    CuEngineAccessor *ea = root->findChild<CuEngineAccessor *>();
    if(!ea) {
        QMessageBox::information(this, "Error: unsupported", "This application does not support engine hot switch");
    }
    else {
        connect(ea, SIGNAL(engineChanged(QStringList, QStringList)), this, SLOT(onEngineChanged(QStringList, QStringList)));
        // get current engine in use
        CuModuleLoader ml;
        int engine = ml.engine_type(ctxi->getContext());
        QGridLayout *gblo = new QGridLayout(this);
        gblo->setObjectName("main_lo");
        if(engine == CumbiaTango::CumbiaTangoType || engine == CumbiaHttp::CumbiaHTTPType) {

            QRadioButton *rbn = new QRadioButton("native", this);
            rbn->setObjectName("rbn");

            QRadioButton *rbh = new QRadioButton("http", this);
            rbh->setObjectName("rbh");

#ifndef QUMBIA_TANGO_CONTROLS_VERSION
            rbn->setDisabled(true);
#endif
#ifndef CUMBIA_HTTP_VERSION
            rbh->setDisabled(true);
#endif

            if(findChildren<QRadioButton *>().size() > 1) {
                QLabel *la = new QLabel("switch between the available engines", this);
                la->setObjectName("labswitch");
                gblo->addWidget(la, 0, 0, 1, 4);
            }
            if(engine == CumbiaTango::CumbiaTangoType && findChild<QRadioButton *>("rbn"))
                findChild<QRadioButton *>("rbn")->setChecked(true);
            else if(engine == CumbiaHttp::CumbiaHTTPType && findChild<QRadioButton *>("rbh"))
                findChild<QRadioButton *>("rbh")->setChecked(true);

            // add radios to layout
            if(findChild<QRadioButton *>("rbn"))
                gblo->addWidget(findChild<QRadioButton *>("rbn"), 1, 2, 1, 1);
            if(findChild<QRadioButton *>("rbh"))
                gblo->addWidget(findChild<QRadioButton *>("rbh"), 1, 3, 1, 1);
            foreach(QRadioButton *rb, findChildren<QRadioButton *>()) {
                connect(rb, SIGNAL(toggled(bool)), this, SLOT(switchEngine(bool)));
            }
        }
        else {
            QLabel *l = new QLabel(QString("error: detected engine is neither tango nor http: %1").arg(engine), this);
            gblo->addWidget(l, 0, 0, 1, 10);
        }
        m_resizeToMinimumSizeHint();
        show();
        printf("dialog shown\n");
        foreach(QWidget *w, findChildren<QWidget *>())
            printf("dialog w %s\n", qstoc(w->objectName()), w->metaObject()->className());
    }
}

void CuEngineSwitchDialog::switchEngine(bool checked) {
    if(checked) {
        QObject *root = root_obj(d->owner);
        CuModuleLoader ml;
        CuControlsFactoryPool fp;
        pretty_pri("d->ctxi %p", d->ctxi);
        pretty_pri("d->ctxi->getContext %p", d->ctxi->getContext());
        CumbiaPool *cu_pool = d->ctxi->getContext()->cumbiaPool();

        pretty_pri("cu_pool is %p, root obj is %s (%s)", cu_pool, root->metaObject()->className(), qstoc(root->objectName()));

        if(cu_pool && root) {
            fp = d->ctxi->getContext()->getControlsFactoryPool();
            int engine = (sender()->objectName() == "rbn" ? static_cast<int>(CumbiaTango::CumbiaTangoType)
                                                          : static_cast<int>(CumbiaHttp::CumbiaHTTPType));
            bool ok = ml.switch_engine(engine, cu_pool, fp, root);
            if(!ok) {
                findChild<QLabel *>("labswitch")->setWordWrap(true);
                findChild<QLabel *>("labswitch")->setText(ml.msg());
                perr("%s", qstoc(ml.msg()));
            }
        }
    }
}

QObject *CuEngineSwitchDialog::root_obj(QObject *leaf) {
    QObject *root = leaf;
    while(root->parent())
        root = root->parent();
    return root != leaf ? root : nullptr;
}
