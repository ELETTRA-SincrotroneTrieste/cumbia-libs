#ifndef QULOGIMPL_H
#define QULOGIMPL_H

#include <QDialog>
#include <culog.h>

class QuLogImplPrivate;

/*! @private
 */
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
 *
 * This class implements CuLogImplI to add log messages and show a dialog in determined conditions.
 * The write method has a CuLog::Class input parameter that can be used in conjunction with showPopupOnMessage
 * to determine when the dialog window must be shown.
 * By default, CuLog::Write class log entries pop up the dialog. This can be disabled by calling
 * \code
 * my_qulog_impl.showPopupOnMessage(CuLog::Write, false);
 * \endcode
 * CuLog::Write class type is meant to categorize entries generated by *write operations*, to provide
 * clear feedback to the user.
 *
 * To know whether a message of a given CuLog::Class will pop up the dialog or not, you can check the
 * result from popupOnMessageClass.
 *
 * The *type of message* is characterized by the MsgType enum values. In QuLogImpl the type is displayed
 * in the first column of each message in the dialog view.
 *
 * QuLogImpl::MsgType and CuLog::Class can be user extended for more custom types and classes.
 *
 * If *write* is called with a CuLog::Class not configured to display the dialog box, the latter can be
 * explicitly shown by calling
 *
 * \code my_qulog_impl.getDialog()->show(); \endcode
 */
class QuLogImpl : public CuLogImplI, public QuErrorDialogListener
{
public:
    enum MsgType { Info = 0, Warn, Error, User = 100 };

    QuLogImpl();

    void showPopupOnMessage(int log_class_type, bool show);
    bool popupOnMessageClass(int log_class_type) const;

    QuErrorDialog *getDialog();

    virtual ~QuLogImpl();

private:
    QuLogImplPrivate * d;

    // QuErrorDialogListener interface
public:
    void onDialogDeleted();

    // CuLogImplI interface
public:
    void write(const std::string &origin, const std::string &write, int l, int c);
    std::string getName() const;
};

#endif // QUERRORDIALOG_H
