#include "qulogimpl.h"
#include <cumacros.h>
#include <QTreeWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QList>

QuLogEntry::QuLogEntry(const QString &ty, const QString &orig, const QString &msg)
{
    type = ty;
    origin = orig;
    message = msg;
}

/** @private */
class QuLogImplPrivate
{
  public:
    QuErrorDialog *dialog;
    QList<QuLogEntry> msgs;
    QMap<int, bool> showPopupByLogClassMap;
};

QuErrorDialog::QuErrorDialog(QWidget *parent,
                             QuErrorDialogListener *l,
                             const QList<QuLogEntry> &elist)
    : QDialog(parent)
{
    m_dialogListener = l;
    setAttribute(Qt::WA_DeleteOnClose, true);
    resize(560, 380);
    QGridLayout *lo = new QGridLayout(this);
    QTreeWidget *tw = new QTreeWidget(this);
    QPushButton *pb = new QPushButton("Close", this);
    QPushButton *pbClear = new QPushButton("Clear");

    lo->addWidget(tw, 0, 0, 5, 10);
    lo->addWidget(pbClear, 5, 8, 1, 1);
    lo->addWidget(pb, 5, 9, 1, 1);

    connect(pbClear, SIGNAL(clicked()), tw, SLOT(clear()));
    connect(pb, SIGNAL(clicked()), this, SLOT(close()));

    tw->setHeaderLabels(QStringList() << "origin" << "message");

    add(elist);
}

QuErrorDialog::~QuErrorDialog()
{
    cuprintf("~QuErrorDialog");
    m_dialogListener->onDialogDeleted();
}

void QuErrorDialog::add(const QList<QuLogEntry> &l)
{
    foreach(QuLogEntry e, l)
        add(e);
}

void QuErrorDialog::add(const QuLogEntry &l)
{
    QTreeWidgetItem *par = NULL;
    QList<QTreeWidgetItem *> its = findChild<QTreeWidget *>()->findItems(l.origin, Qt::MatchExactly, 0);
    if(its.size() > 0) /* found item with the same "origin" */
        par = its.first();
    else
        par = new QTreeWidgetItem(findChild<QTreeWidget *>(), QStringList() << l.origin);

    par->setExpanded(true);
    QString shortMsg = l.message;
    shortMsg.replace("\n", "/");
    shortMsg.truncate(200);
    QTreeWidgetItem *i = new QTreeWidgetItem(par, QStringList() << l.type << shortMsg );
    i->setToolTip(1, l.message);
    if(l.type == "ERR")
        i->setData(0, Qt::UserRole, QuLogImpl::Error);
    else if(l.type == "WARN")
        i->setData(0, Qt::UserRole, QuLogImpl::Warn);
    else
        i->setData(0, Qt::UserRole, QuLogImpl::Info);

    switch(i->data(0, Qt::UserRole).toInt())
    {
    case QuLogImpl::Error:
        i->setTextColor(0, QColor(Qt::red));
        break;
    case QuLogImpl::Warn:
        i->setTextColor(0, QColor(Qt::yellow));
        break;
    default:
        break;
    }
}

QuLogImpl::QuLogImpl()
{
    d = new QuLogImplPrivate;
    d->dialog = nullptr;
    d->showPopupByLogClassMap[CuLog::Write] = true;
}

/** \brief Enable/disable popup for the given log_class_type
 *
 * @param log_class_type: a value from CuLogClass::Type enum or its extension
 * @param show show a popup when a new error occurse for the given CuLogClass::Type type
 *
 */
void QuLogImpl::showPopupOnMessage(int log_class_type, bool show)
{
    d->showPopupByLogClassMap[log_class_type] = show;
}

bool QuLogImpl::popupOnMessageClass(int log_class_type) const
{
    return d->showPopupByLogClassMap[log_class_type];
}

QuErrorDialog *QuLogImpl::getDialog()
{
    if(!d->dialog)
        d->dialog = new QuErrorDialog(nullptr, this, QList<QuLogEntry>());
    return d->dialog;
}

QuLogImpl::~QuLogImpl()
{
    pdelete("~QuLogImpl");
    delete d;
}

void QuLogImpl::write(const std::string &origin, const std::string &msg, CuLog::Level l, CuLog::Class c)
{
    QString type;
    if(l == CuLog::Info) type = "INFO";
    else if(l == CuLog::Warn) type = "WARN";
    else type = "ERR";

    QuLogEntry e(type, QString::fromStdString(origin), QString::fromStdString(msg));
    d->msgs.append(e);
    if(!d->dialog)
        d->dialog = new QuErrorDialog(nullptr, this, d->msgs);
    else
        d->dialog->add(e);
    if(d->showPopupByLogClassMap[c])
        d->dialog->show();
}

void QuLogImpl::write(const std::string &origin, CuLog::Level l, CuLog::Class c, const char *fmt, ...)
{

    va_list vl;
    va_start(vl, fmt);
    char s[2048];QString type;
    if(l == CuLog::Info) type = "INFO";
    else if(l == CuLog::Warn) type = "WARN";
    else type = "ERR";

    vsnprintf(s, 2048, fmt, vl);
    QuLogEntry e(type, QString::fromStdString(origin), QString(s));
    d->msgs.append(e);

    if(!d->dialog)
        d->dialog = new QuErrorDialog(0, this, d->msgs);
    else
        d->dialog->add(e);
    if(d->showPopupByLogClassMap[c])
        d->dialog->show();
    va_end(vl);
}

std::string QuLogImpl::getName() const
{
    return "QuLogImpl";
}


void QuLogImpl::onDialogDeleted()
{
    d->dialog = NULL;
}

