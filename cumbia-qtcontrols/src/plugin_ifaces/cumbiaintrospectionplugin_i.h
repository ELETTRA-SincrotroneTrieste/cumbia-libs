#ifndef CUMBIAINTROSPECTIONPLUGIN_I_H
#define CUMBIAINTROSPECTIONPLUGIN_I_H

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
class CuIntrospectionEngineExtensionPlugin_I;

class ThreadInfo {
public:
    CuData token;
    QStringList devices;
    pthread_t id;
    std::vector<CuActivity *> activities; // activities for thread
};

class CumbiaIntrospectionPlugin_I
{
public:

    virtual ~CumbiaIntrospectionPlugin_I() {}

    virtual void init(Cumbia *cumbia, const QStringList& name_search_keys) = 0;

    virtual int getThreadCount() const = 0;

    virtual void installEngineExtension(CuIntrospectionEngineExtensionPlugin_I *eei) = 0;

    /*!
     * \brief update the whole introspection data
     */
    virtual void update() = 0;

    /*!
     * \brief Find a value among a data bundle to be used as name for the thread.
     *
     * Search for *search_keys* keys within th_tok. The first that is found is returned as *primary key*
     *
     * \param data_tok CuData where to search for *search_keys*
     *
     * The init method sets a list of search_keys strings used to search amongst the keys of data_tok
     *
     * @see init
     *
     * \return the value associated to the first search_keys element found in the data_tok keys.
     *         An empty string if no key is found.
     */
    virtual QString findName(const CuData &data_tok) const = 0;

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
