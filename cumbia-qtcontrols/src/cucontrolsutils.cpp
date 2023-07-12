#include "cucontrolsutils.h"
#include "qustring.h"

#include <QVariant>
#include <QString>
#include <QWidget>
#include <QtDebug>
#include <QApplication>
#include <QMetaProperty>
#include <string>
#include <cumacros.h>
#include <cudata.h>
#include <QRegularExpression>

#include <QDateTime>
#include <QStringBuilder>
#include <QStringLiteral>
#include <QtDebug>

QRegularExpression re("\\((.*)\\)");

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
QString CuControlsUtils::findInput(const QString &objectName, const QObject *leaf, bool *found) const
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
    if((*found = (o && o->metaObject()->indexOfProperty("data") > -1)))
        ret = o->property("data").toString();
    else if((*found = (o && o->property("indexMode").toBool() && o->metaObject()->indexOfProperty("currentIndex") >= 0)))
        ret = QString("%1").arg(o->property("currentIndex").toInt() + o->property("indexOffset").toInt());
    else if((*found = (o && o->metaObject()->indexOfProperty("text") > -1)))
        ret = o->property("text").toString();
    else if((*found = (o && o->metaObject()->indexOfProperty("value") > -1)))
        ret = o->property("value").toString();
    else if((*found = (o && o->metaObject()->indexOfProperty("currentText") > -1)))
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
    QRegularExpressionMatch match = re.match(target);
    if(match.captured().size() > 0) {
        QString argums = match.captured(1);
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
        QStringList args = argums.split(",", Qt::SkipEmptyParts);
#else
        QStringList args = argums.split(",", QString::SkipEmptyParts);
#endif
        foreach(QString a, args) {
            if(a.startsWith("&"))  {
                bool found;
                oName = a.remove(0, 1);
                val = findInput(oName, leaf, &found);
                if(found) {
                    argins.push_back(val.toStdString());
                }
                else
                    perr("CuControlsUtils.getArgs: no object named \"%s\" found", qstoc(oName));
            }
            else
                argins.push_back(a.toStdString());
        }
        if(args.size() > 1)
            return CuVariant(argins);
        else if(args.size() == 1 && argins.size() > 0)
            return CuVariant(argins.at(0));
    }
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
        else {
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
    QRegularExpressionMatch ma = re.match(target);
    if(ma.captured().size() > 0)  {
        QString argums = ma.captured(1);
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
        QStringList args = argums.split(",", Qt::SkipEmptyParts);
#else
        QStringList args = argums.split(",", QString::SkipEmptyParts);
#endif
        foreach(QString a, args) {
            if(a.startsWith("&")) {
                oName = a.remove(0, 1);
                QObject* o = findObject(oName, leaf);
                if(o)
                    objects << o;
            }
        }
    }
    return objects;
}

/*! \brief initialize objects setting range (min, max, if relevant) and the value according to value_key
 *
 * @param target the name of the target
 * @param leaf the parent object under which children are searched by findObjects
 * @param data the CuData holding configuration values (min, max, if present), and a value to display on the object
 * @param value_key the key to look for the value that must be displayed (example: "value" or "w_value")
 *
 * @return true if the operation was successful, false otherwise
 */
bool CuControlsUtils::initObjects(const QString &target, const QObject *leaf, const CuData &data, const char* value_key)
{
    bool ret, has_target;
    int idx, match = 0;
    double min = -1, max = -1;
    int data_siz = -1;
    CuControlsUtils cu;
    int vtype;
    std::string value_as_str;
    std::vector<std::string> values_str;
    const CuVariant& val = data[value_key];

    if(val.isValid())
        data_siz = static_cast<int>(val.getSize());
    // min, max
    if(data[CuDType::Max].isValid() && data[CuDType::Min].isValid()) {
        data[CuDType::Max].to<double>(max);
        data[CuDType::Min].to<double>(min);
    }
    QList<QObject *> inputobjs = cu.findObjects(target, leaf);
    CuVariant::DataFormat fmt = val.getFormat();
    if(fmt == CuVariant::Scalar)
        value_as_str = val.toString(&ret);
    else
        values_str = val.toStringVector(&ret);

    for(int i = 0; i < inputobjs.size() && ret; i++) {
        QObject *o = inputobjs[i];
        has_target = o->metaObject()->indexOfProperty("target") >= 0 && !o->property("target").toString().isEmpty();
        if(!has_target) {  // initialise
            ret = true;
            std::string vs;
            const QMetaObject *mo = o->metaObject();

            // min, max, before value
            if( (min != max) &&  (idx = mo->indexOfProperty("minimum")) > -1 && mo->property(idx).isWritable() &&
                (idx = mo->indexOfProperty("maximum")) > -1 && mo->property(idx).isWritable()) {
                ret = o->setProperty("minimum", min) && o->setProperty("maximum", max);
            }

            if(i < data_siz) {
                fmt == CuVariant::Scalar ? vs = value_as_str : vs = values_str[i];
                if((idx = mo->indexOfProperty("text") ) > -1 && mo->property(idx).isWritable()) {
                    ret = o->setProperty("text", QString::fromStdString(vs));
                }
                else if((idx = mo->indexOfProperty("value") ) > -1 && mo->property(idx).isWritable()) {
                    try {
                        // QVariant.typeId: returns the storage type of the value stored in the variant.
                        // This is the same as metaType().id().
                        vtype = mo->property(idx).type();
                        if(vtype == QMetaType::Double)
                            ret =o->setProperty("value", strtod(vs.c_str(), NULL));
                        else if(vtype == QMetaType::Int)
                            ret =o->setProperty("value", strtoll(vs.c_str(), NULL, 10));
                        else if(vtype == QMetaType::UInt)
                            ret =o->setProperty("value", static_cast<unsigned int>(strtoul(vs.c_str(), NULL, 10)));
                        else if(vtype == QMetaType::Bool)
                            ret =o->setProperty("value", vs != "0" && strcasecmp(vs.c_str(), "false") != 0);
                        else {
                            perr("cannot set prop value cuz type %d not supported", vtype);
                        }
                    }
                    catch(const std::invalid_argument &ia) {
                        perr("CuControlsUtils.initObjects: could not convert \"%s\" to a number", vs.c_str());
                    }
                }
                else if(( idx = mo->indexOfProperty("currentText") ) > -1 && mo->property(idx).isWritable())
                    ret = o->setProperty("currentText", QString::fromStdString(val.toString()));
                else if(( idx = mo->indexOfProperty("checked")) > -1 && mo->property(idx).isWritable())
                    ret = o->setProperty("value", vs != "0" && strcasecmp(vs.c_str(), "false") != 0);
            } // i < data_siz

            if(!ret)
                perr("CuControlsUtils.initObjects: failed to set value \"%s\" on object \"%s\"", vs.c_str(), qstoc(o->objectName()));
            else
                match++;

        }
    }
    return inputobjs.size() == match;
}

/*!
 * \brief return a string with a message derived from the da input parameter
 * \param da CuData
 * \param date_time_fmt a custom timestamp date / time format. Default: "yyyy-MM-dd HH:mm:ss.zzz"
 *
 * \return the message stored in da[CuDType::Message] if da[CuDType::Err] evaluates to true or  // da["msg"], da["err"]
 *  a new message with the source name, the value of da[CuDType::Mode] (or da[CuDType::Activity], if CuDType::Mode is empty) and the timestamp.  // da["mode"], da["activity"]
 *  da[CuDType::Time_ms] or da[CuDType::Time_us] are used to provide a date /time in the format "yyyy-MM-dd HH:mm:ss.zzz".  // da["timestamp_ms"], da["timestamp_us"]
 */
QString CuControlsUtils::msg(const CuData &da, const QString& date_time_fmt) const {
    QString m;
    const std::string& src = da.s(CuDType::Src); // determines the str len mostly
    const std::string& msg = da.s(CuDType::Message);
    const int reservesiz = !da.B(CuDType::Err) ? src.length() + msg.length() + 30 : msg.length() + src.length() + 30;
    m.reserve(reservesiz);
    m = src.c_str();
    // timestamp
    long int ts = 0;

    da[CuDType::Time_ms].to<long int>(ts);
    if(ts > 0)
        m += " " + QDateTime::fromMSecsSinceEpoch(ts).toString(date_time_fmt);
    if(ts == 0) {
        double tsd = 0.0;
        da[CuDType::Time_us].to<double>(tsd);  // secs.usecs in a double
        if(tsd > 0)
            m += " " + QDateTime::fromMSecsSinceEpoch(static_cast<long int>(tsd * 1000)).toString(date_time_fmt);
    }

    if(msg.length() > 0) {
        m += QStringLiteral(": ") + msg.c_str();
    }
    else {
        // pick mode or activity name
        const std::string& _mode = da.s(CuDType::Mode);
        if(_mode.length() > 0)
            m += (" [" + QuString(_mode) + "] ");
        else
            m += (" [" + QuString(da, CuDType::Activity) + "] ");
    }

    if(m.length() > reservesiz)
        printf("CuControlsUtils::msg: reserved size was \e[1;32m%d\e[0m, actual string len is \e[0;32m%d\e[0m\t\e[1;31m REALLOC NEEDED\e[0m\n", reservesiz, m.length());

    return m;
}

