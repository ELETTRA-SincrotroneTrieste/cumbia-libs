#ifndef QULOGIMPL_H
#define QULOGIMPL_H

#include <QDialog>
#include <culog.h>

class QuLogImplPrivate;

class QuErrorDialogListener
{
public:
    virtual void onDialogDeleted() = 0;
};

/*! \brief cumbia-qtcontrols error dialog
 *
 * \ingroup log
 */
class QuLogEntry
{
public:
    QuLogEntry(const QString &ty, const QString& orig, const QString& msg);

    QString type, origin, message;
};

/*! \brief cumbia-qtcontrols error dialog
 *
 * \ingroup log
 */
class QuErrorDialog : public QDialog
{
public:

    QuErrorDialog(QWidget *parent,
                  QuErrorDialogListener *l,
                  const QList<QuLogEntry> &elist = QList<QuLogEntry>());

    virtual ~QuErrorDialog();

    void add(const QList<QuLogEntry> &l);

    void add(const QuLogEntry &l);

private:
    QuErrorDialogListener *m_dialogListener;

};

/*! \brief cumbia-qtcontrols CuLogImplI log implementation
 *
 * \ingroup log
 */
class QuLogImpl : public CuLogImplI, public QuErrorDialogListener
{
public:
    enum MsgType { Info = 0, Warn, Error, User = 100 };

    QuLogImpl();

    void showPopupOnMessage(int log_class_type, bool show);

    virtual ~QuLogImpl();

private:
    QuLogImplPrivate * d;

    // QuErrorDialogListener interface
public:
    void onDialogDeleted();

    // CuLogImplI interface
public:
    void write(const std::string &origin, const std::string &write, CuLog::Level l, CuLog::Class c);
    void write(const std::string &origin, CuLog::Level l, CuLog::Class c, const char *fmt, ...);
    std::string getName() const;
};

#endif // QUERRORDIALOG_H
