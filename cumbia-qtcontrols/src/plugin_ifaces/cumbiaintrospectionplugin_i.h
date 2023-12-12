#ifndef CUMBIAINTROSPECTIONPLUGIN_I_H
#define CUMBIAINTROSPECTIONPLUGIN_I_H

// test 1

#include <cudata.h>
#include <QStringList>
#include <QMap>
#include <vector>
#include <QObject>

class CuActivity;
class QStandardItemModel;
class Cumbia;
class QDialog;
class QStandardItem;
class CuTimerListener;
class CuIntrospectionEngineExtensionI;

class TimerInfo {
public:
    QString name;
    int timeout;
    QList<CuTimerListener *> timer_listeners;
};

class ThreadInfo {
public:
    std::string token;
    QStringList devices;
    std::vector<CuActivity *> activities; // activities for thread
};

class CuIntrospectionEngineExtensionI {
public:
    virtual ~CuIntrospectionEngineExtensionI() {}

    /*!
     * \brief Specific engines can provide information about each activity adding rows represented of a list
     *        of QStandardItem that will be children of the activity row
     *
     * \param a CuActivity
     *
     * \return A list of *list of items* that will be children rows of *a*.
     *
     * The default implementation returns an empty list.
     *
     * @see columnCount
     */
    virtual QList<QList<QStandardItem *> > activityChildRows(const CuActivity *a) const = 0;

    /*!
     * \brief Returns the column count to set on the model
     *
     * The default implementation returns 2. Subclasses will return another value
     * if activityChildItems requires a greater number of columns.
     *
     * @see activityChildItems
     */
    virtual int modelColumnCount() const = 0;

    virtual QStringList modelHeaderLabels() const = 0;

    virtual QString dialogHeading() const = 0;
};

class CumbiaIntrospectionPlugin_I
{
public:

    virtual ~CumbiaIntrospectionPlugin_I() {}

    virtual void init(Cumbia *cumbia) = 0;

    virtual int getThreadCount() const = 0;

    virtual void installEngineExtension(CuIntrospectionEngineExtensionI *eei) = 0;

    /*!
     * \brief update the whole introspection data
     */
    virtual void update() = 0;

    /*!
     * \brief getThreadInfo returns a map associating the *name* found in the thread token to the thread info.
     *
     * \return a map linking a thread name to its info
     */
    virtual QMap<QString, ThreadInfo> getThreadInfo() = 0;

    virtual const ThreadInfo getThreadInfo(const QString& name) = 0;

    /*!
     * \brief Returns a list of errors encountered during either the *update* process or *toItemModel*
     *
     */
    virtual QStringList errors() const = 0;

    /*!
     * \brief toItemModel provides a hierarchical representation of the data ready to be used in a tree view
     *
     * \return a new QStandardItemModel storing a hierarchical representation of the data
     */
    virtual QStandardItemModel *toItemModel() const = 0;


    virtual QDialog *getDialog(QWidget *parent) = 0;

};

#define CumbiaIntrospectionPlugin_I_iid "eu.elettra.qutils.CumbiaIntrospectionPlugin_I"

Q_DECLARE_INTERFACE(CumbiaIntrospectionPlugin_I, CumbiaIntrospectionPlugin_I_iid)

#endif // CUMBIAINTROSPECTIONPLUGIN_I_H
