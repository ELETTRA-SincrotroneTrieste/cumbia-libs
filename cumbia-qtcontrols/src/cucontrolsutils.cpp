#include "cucontrolsutils.h"

#include <QVariant>
#include <QString>
#include <QWidget>
#include <QtDebug>
#include <QApplication>
#include <QMetaProperty>
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
 * The object providing input arguments must implement one of the following properties, that
 * are searched and evaluated in this order:
 *
 * \li *data* (e.g. QuComboBox: data represented as string, either currentText or currentIndex
 *     according to QuComboBox indexMode property)
 * \li *text* (e.g. QLabel, QLineEdit, ...)
 * \li *value* (e.g. QSpinBox, QDoubleSpinBox, ENumeric)
 * \li *currentIndex* (e.g. QComboBox)
 *
 * \note
 * The first property found in the list above is used. If you write a custom widget that displays data
 * that may be internally represented by different types (e.g. a combo box that can be used to either change
 * a numerical index or a text), make sure to provide a *data* property that can be converted to QString
 * regardless of the internal storage type.
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
    if(o && o->metaObject()->indexOfProperty("data") > -1)
        ret = o->property("data").toString();
    else if(o && o->metaObject()->indexOfProperty("text") > -1)
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

QObject *CuControlsUtils::findObject(const QString &objectName, const QObject *leaf) const
{
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

    return o;
}

QList<QObject *> CuControlsUtils::findObjects(const QString& target, const QObject *leaf)
{
    QList<QObject *> objects;
    QString oName;
    cuprintf("\e[1;34mgetArgs finding args in %s\e[0m\n\n", qstoc(target));
    QRegExp re("\\((.*)\\)");
    int pos = re.indexIn(target);
    if(pos < 0)
        return objects;
    QString argums = re.cap(1);
    QStringList args = argums.split(",", QString::SkipEmptyParts);
    foreach(QString a, args)
    {
        if(a.startsWith("&"))
        {
            oName = a.remove(0, 1);
            QObject* o = findObject(oName, leaf);
            if(o)
                objects << o;
        }
    }
    return objects;
}

bool CuControlsUtils::initObjects(const QString &target, const QObject *leaf, const CuVariant &val)
{
    bool ret, has_target;
    int idx, match = 0;
    int siz = static_cast<int>(val.getSize());
    CuControlsUtils cu;
    QVariant::Type vtype;
    std::string value_as_str;
    std::vector<std::string> values_str;
    QList<QObject *> inputobjs = cu.findObjects(target, leaf);
    printf("found %d objects associated to target %s object %s VAL %s\n",
           inputobjs.size(), qstoc(target), qstoc(leaf->objectName()), val.toString().c_str());
    CuVariant::DataFormat fmt = val.getFormat();
    if(fmt == CuVariant::Scalar)
        value_as_str = val.toString(&ret);
    else
        values_str = val.toStringVector(&ret);

    printf("objects found %d val size %d ret %d\n", inputobjs.size(), siz, ret);
    for(int i = 0; i < inputobjs.size() && i < siz && ret; i++) {
        QObject *o = inputobjs[i];
        has_target = o->metaObject()->indexOfProperty("target") >= 0 && !o->property("target").toString().isEmpty();
        printf("CuControlsUtils.initObjects: object %s class %s has targets %d index %d prop %s\e[0m\n", qstoc(o->objectName()), o->metaObject()->className(),
               has_target,o->metaObject()->indexOfProperty("target"), qstoc(o->property("target").toString()) );
        if(!has_target) {  // initialise
            {
                printf("\e[1;32mCuControlsUtils.initObjects: object %s class %s has NO targets\e[0m\n", qstoc(o->objectName()), o->metaObject()->className() );
                ret = false;
                std::string vs;
                fmt == CuVariant::Scalar ? vs = value_as_str : vs = values_str[i];
                if(o->metaObject()->indexOfProperty("text") > -1)
                    ret = o->setProperty("text", QString::fromStdString(vs));
                else if((idx = o->metaObject()->indexOfProperty("value") ) > -1) {
                    try {
                        vtype = o->metaObject()->property(idx).type();
                        if(vtype == QVariant::Double)
                            ret =o->setProperty("value", strtod(vs.c_str(), NULL));
                        else if(vtype == QVariant::Int)
                            ret =o->setProperty("value", strtoll(vs.c_str(), NULL, 10));
                        else if(vtype == QVariant::UInt)
                            ret =o->setProperty("value", static_cast<unsigned int>(strtoul(vs.c_str(), NULL, 10)));
                        else if(vtype == QVariant::Bool)
                            ret =o->setProperty("value", vs != "0" && strcasecmp(vs.c_str(), "false") != 0);
                    }
                    catch(const std::invalid_argument &ia) {
                        perr("CuControlsUtils.initObjects: could not convert \"%s\" to a number", vs.c_str());
                    }
                }
                else if(o->metaObject()->indexOfProperty("currentText") > -1)
                    ret = o->setProperty("currentText", QString::fromStdString(val.toString()));
                else if(o->metaObject()->indexOfProperty("checked") > -1)
                    ret = o->setProperty("value", vs != "0" && strcasecmp(vs.c_str(), "false") != 0);

                if(!ret)
                    perr("CuControlsUtils.initObjects: failed to set value \"%s\" on object \"%s\"", vs.c_str(), qstoc(o->objectName()));
                else
                    match++;
            }
        }
    }
    return inputobjs.size() == match;
}

