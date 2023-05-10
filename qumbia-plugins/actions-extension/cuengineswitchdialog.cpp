#include "cuengineswitchdialog.h"
#include "cuengineaccessor.h"
#include <QMessageBox>
#include <QLabel>
#include <QRadioButton>
#include <qgridlayout.h>
#include <quapps.h>
#include <cucontexti.h>
#include <cucontext.h>
#include <cuengine_swap.h>

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

void CuEngineSwitchDialog::m_err_notify(const QString &msg) const {
    findChild<QLabel *>("labswitch")->setWordWrap(true);
    findChild<QLabel *>("labswitch")->setText(msg);
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

void CuEngineSwitchDialog::m_owner_destroyed(QObject *) {
    this->close();
}

void CuEngineSwitchDialog::exec(const CuData &in, const CuContextI *ctxi)
{
    d->owner = static_cast<QObject *>(in["sender"].toVoidP());
    printf("ctxi %p  d->ctxi %p\n", ctxi, d->ctxi);
    //
    // engine hot switch (since 1.5.0)
    //
    QObject *root = root_obj(d->owner);
    connect(d->owner, SIGNAL(destroyed(QObject *)), this, SLOT(m_owner_destroyed(QObject*)));
    CuEngineAccessor *ea = root->findChild<CuEngineAccessor *>();
    if(!ea) {
        QMessageBox::information(this, "Error: unsupported", "This application does not support engine hot switch");
    }
    else {
        connect(ea, SIGNAL(engineChanged(QStringList, QStringList)), this, SLOT(onEngineChanged(QStringList, QStringList)));
        // get current engine in use
        CuEngineSwap ctxsw;
        int engine = ctxsw.engine_type(ctxi->getContext());
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
    }
}

void CuEngineSwitchDialog::switchEngine(bool checked) {
    if(checked) {
        QObject *root = root_obj(d->owner);
        CuModuleLoader ml;
        CuEngineSwap ctxsw;
        pretty_pri("d->ctxi %p", d->ctxi);
        pretty_pri("d->ctxi->getContext %p", d->ctxi->getContext());
        CumbiaPool *old_pool = d->ctxi->getContext()->cumbiaPool();

        pretty_pri("cu_pool is %p, root obj is %s (%s)", old_pool, root->metaObject()->className(), qstoc(root->objectName()));

        if(old_pool && root) {
            CuControlsFactoryPool old_fap = d->ctxi->getContext()->getControlsFactoryPool();
            CuControlsFactoryPool new_fap;
            int engine = (sender()->objectName() == "rbn" ? static_cast<int>(CumbiaTango::CumbiaTangoType)
                                                          : static_cast<int>(CumbiaHttp::CumbiaHTTPType));
            bool ok = !ctxsw.same_engine(old_pool, engine); // engine must be different from current
            if(!ok) {
                m_err_notify(ctxsw.msg());
            }
            else {
                ok = ctxsw.check_root_has_engine_accessor(root);
                if(!ok) {
                    m_err_notify(QString("object '%1' class '%2' does not have an engine accessor")
                                 .arg(root->objectName()).arg(root->metaObject()->className()));
                }
                else {
                    QString oclass, oname;
                    // all objects with either source or target shall have the ctx swap dedicated method
                    bool ok = ctxsw.check_objs_have_ctx_swap(root, &oclass, &oname);
                    if(!ok) {
                        m_err_notify(ctxsw.msg());
                    }
                    else {
                        // allocate new_pool and populate new_fap with the desired engine
                        CumbiaPool *new_pool = ml.prepare_engine(engine, &new_fap);
                        if(!new_pool) {
                            m_err_notify(ml.msg());
                            perr("%s", qstoc(ml.msg()));
                        }
                        // gather old Cumbias. old_fap is cleared although not a reference
                        // unregister cumbias from the old_pool
                        std::vector<Cumbia *> old_cums = ctxsw.cumbia_clear(old_pool, old_fap);
                        // if CuLog is installed in current cumbias, move it to the new ones
                        // discard ok: if false it may simply indicate log instance was not
                        // installed in cumbias
                        ok = ctxsw.log_move(old_cums, new_pool);
                        // swap context on objects
                        ctxsw.ctx_swap(root, new_pool, new_fap);
                        // swap engine within the whole application
                        ok = ctxsw.app_engine_swap(root, new_pool, new_fap);
                        if(!ok)
                            m_err_notify(ctxsw.msg());
                        else {
                            // delete old cumbias
                            ctxsw.cumbias_delete(old_cums);
                        }
                    }
                }
            }
        }
    }
}

QObject *CuEngineSwitchDialog::root_obj(QObject *leaf) {
    QObject *root = leaf;
    while(root && root->parent()) {
        printf(" ---+---- %p\n", root);
        printf("          %s %s\n", qstoc(root->objectName()), root->metaObject()->className());
        root = root->parent();
    }
    return root != leaf ? root : nullptr;
}
