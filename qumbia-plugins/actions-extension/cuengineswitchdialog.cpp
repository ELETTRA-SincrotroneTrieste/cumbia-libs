#include "cuengineswitchdialog.h"

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

void CuEngineSwitchDialog::exec(const CuData &in, const CuContextI *ctxi)
{
    d->owner = static_cast<QObject *>(in["sender"].toVoidP());
    printf("ctxi %p  d->ctxi %p\n", ctxi, d->ctxi);
    //
    // engine hot switch (since 1.5.0)
    //
    QObject *root = root_obj(d->owner);
    printf("\e[1;32mCuEngineSwitchDialog: root is %s (%s) from leaf %s (%s)\e[0m\n", root != nullptr ? qstoc(root->objectName())
                                                                                                   : "nullptr", root != nullptr ? root->metaObject()->className() : "-", qstoc(d->owner->objectName()), d->owner->metaObject()->className());
    // get current engine in use
    CuModuleLoader ml;
    int engine = ml.engine_type(ctxi->getContext());
    if(engine == CumbiaTango::CumbiaTangoType || engine == CumbiaHttp::CumbiaHTTPType) {
        QGridLayout *gblo = new QGridLayout(this);
#ifdef QUMBIA_TANGO_CONTROLS_VERSION
        QRadioButton *rbn = new QRadioButton("native", this);
        rbn->setObjectName("rbn");
#endif
#ifdef CUMBIA_HTTP_VERSION
        QRadioButton *rbh = new QRadioButton("http", this);
        rbh->setObjectName("rbh");
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
    m_resizeToMinimumSizeHint();
    show();
}

void CuEngineSwitchDialog::switchEngine(bool checked) {
    if(checked) {
        QObject *root = root_obj(d->owner);
        CuModuleLoader ml;
        CuControlsFactoryPool fp;
        pretty_pri("d->ctxi %p", d->ctxi);
        pretty_pri("d->ctxi->getContext %p", d->ctxi->getContext());
        CumbiaPool *cu_pool = d->ctxi-> getContext()-> cumbiaPool();

        pretty_pri("cu_pool is %p, root obj is %s (%s)", cu_pool, root->metaObject()->className(), qstoc(root->objectName()));

        if(cu_pool && root) {
            fp = d->ctxi->getContext()->getControlsFactoryPool();
            int engine = (sender()->objectName() == "rbn" ? static_cast<int>(CumbiaTango::CumbiaTangoType)
                                                          : static_cast<int>(CumbiaHttp::CumbiaHTTPType));
            bool ok = ml.switch_engine(engine, cu_pool, fp, root);
            if(!ok) {
                findChild<QLabel *>("labswitch")->setWordWrap(true);
                findChild<QLabel *>("labswitch")->setText(ml.msg());
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
