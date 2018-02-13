#include "cucontrolsutils.h"

#include <QVariant>
#include <QString>
#include <QWidget>
#include <QtDebug>
#include <QApplication>

#include <string>
#include <cumacros.h>

#include <QtDebug>

CuControlsUtils::CuControlsUtils()
{

}

/*! find the input argument from an object and return it in the shape of a string.
 *
 * @param objectName the name of the object to search for
 * @param leaf the parent object under which the object providing input is searched for
 *
 * @return a string with the argument.
 *
 * \note
 * The object providing input arguments must implement one of the following properties:
 * \li text (e.g. QLabel, QLineEdit, ...)
 * \li value (e.g. QSpinBox, QDoubleSpinBox, ENumeric)
 * \li currentIndex (e.g. QComboBox)
 *
 */
QString CuControlsUtils::findInput(const QString &objectName, const QObject *leaf) const
{
    QString ret;
    QObject *parent = leaf->parent();
    QObject *o = NULL;
    while(parent && !o)
    {
        qDebug() << __FUNCTION__ << "parent " << parent << " o " << o;
        if(parent->objectName() == objectName)
            o = parent;
        else
        {
            o = parent->findChild<QObject *>(objectName);
            qDebug() << "findind child " << objectName << " under " << parent << "AMONGST" << " FOUND " << o;
            foreach(QObject *c, parent->findChildren<QObject *>())
                qDebug() << "---" << c << c->objectName();
        }

        parent = parent->parent();
    }
    if(!o) /* last resort: search among all qApplication objects */
    {
        pwarn("CuControlsUtils.findInputProvider: object \"%s\" not found amongst ancestors:\n"
              "  looking for a child under the qApp active window.\n"
              "  Please reorganise QObjects hierarchy for better performance.", qstoc(objectName));
        o = qApp->activeWindow()->findChild<QObject*>(objectName);
    }
    // let cumbia-qtcontrols findInput deal with labels, line edits, combo boxes (currentText), spin boxes
    // and our Numeric
    if(o && o->metaObject()->indexOfProperty("text") > -1)
        ret = o->property("text").toString();
    else if(o && o->metaObject()->indexOfProperty("value") > -1)
        ret = o->property("value").toString();
    else if(o && o->metaObject()->indexOfProperty("currentText") > -1)
        ret = o->property("currentText").toString();

    return ret;
}

/*! \brief finds the arguments in the source or target. If they are object names it finds the
 *         objects with those names and get the input from them, otherwise they are directly
 *         converted to CuVariant and returned.
 *
 * @param target the source or target string
 * @param leaf under this object getArgs searches for a child (children) with the object
 *        name(s) specified in the argument (s).
 *
 * @return a CuVariant with the detected argument(s), or an empty CuVariant.
 *
 * The child is searched by the findInput method.
 */
CuVariant CuControlsUtils::getArgs(const QString &target, const QObject *leaf) const
{
    std::vector<std::string> argins;
    QString oName;
    QString val;
    cuprintf("\e[1;34mgetArgs finding args in %s\e[0m\n\n", qstoc(target));
    QRegExp re("\\((.*)\\)");
    int pos = re.indexIn(target);
    if(pos < 0)
        return CuVariant();
    QString argums = re.cap(1);

    QStringList args = argums.split(",", QString::SkipEmptyParts);
    foreach(QString a, args)
    {
        if(a.startsWith("&"))
        {
            oName = a.remove(0, 1);
            val = findInput(oName, leaf);
            if(!val.isEmpty())
                argins.push_back(val.toStdString());
            else
                perr("CuControlsUtils.getArgs: no object named \"%s\" found", qstoc(oName));
        }
        else
            argins.push_back(a.toStdString());
    }
    for(size_t i = 0; i < argins.size(); i++)
        cuprintf("argin %d args.size %d: %s\n", i, args.size(), argins.at(i).c_str());
    if(args.size() > 1)
        return CuVariant(argins);
    else if(args.size() == 1 && argins.size() > 0)
        return CuVariant(argins.at(0));

    return CuVariant();
}
