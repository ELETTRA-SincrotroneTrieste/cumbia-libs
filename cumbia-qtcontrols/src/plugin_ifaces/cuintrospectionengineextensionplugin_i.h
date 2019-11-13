#ifndef CUINTROSPECTIONENGINEEXTENSIONPLUGIN_I_H
#define CUINTROSPECTIONENGINEEXTENSIONPLUGIN_I_H

#include <QObject>
#include <QStringList>

class CuActivity;
class QStandardItem;

class CuIntrospectionEngineExtensionPlugin_I {
public:
    virtual ~CuIntrospectionEngineExtensionPlugin_I() {}

    /*!
     * \brief Specific engines can provide information about each activity by means of a list
     *        of QStandardItem that will be a row child of the activity row
     *
     * \param a CuActivity
     *
     * \return A list of items that will be child of *a*.
     *
     * The default implementation returns an empty list.
     *
     * @see columnCount
     */
    virtual QList<QStandardItem *> activityChildItems(const CuActivity *a) const = 0;

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

#define CuIntrospectionEngineExtensionPlugin_I_iid "eu.elettra.qutils.CuIntrospectionEngineExtensionPlugin_I"

Q_DECLARE_INTERFACE(CuIntrospectionEngineExtensionPlugin_I, CuIntrospectionEngineExtensionPlugin_I_iid)


#endif // CUINTROSPECTIONENGINEEXTENSIONPLUGIN_I_H
