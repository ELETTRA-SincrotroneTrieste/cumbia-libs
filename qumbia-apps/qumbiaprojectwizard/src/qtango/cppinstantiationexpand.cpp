#include "cppinstantiationexpand.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonArray>
#include "definitions.h"
#include <QtDebug>

CppInstantiationExpand::CppInstantiationExpand(const QString &filenam)
    : FileCmd(filenam)
{

}


QString CppInstantiationExpand::process(const QString &input)
{
    QString out = input;
    QList<Subst> substs;
    QFile f(QString(TEMPLATES_PATH) + "/cppinstantiationexp.json");
    m_err = !f.open(QIODevice::ReadOnly|QIODevice::Text);
    if(!m_err) {
        QByteArray content = f.readAll();
        QJsonParseError pe;
        QJsonDocument jd = QJsonDocument::fromJson(content, &pe);
        m_err = (pe.error != QJsonParseError::NoError);
        if(m_err)
            m_msg = "CppInstantiationExpand.process" + pe.errorString() + ": offset: " + QString::number(pe.offset);
        else {
            QJsonValue root = jd.object().value("array");
            m_err = root.isUndefined() || root.isNull() || !root.isArray();
            if(!m_err) {
                QJsonArray array = root.toArray();
                for(int i = 0; i < array.size(); i++) {
                    QJsonValue jv = array.at(i);
                    QJsonObject jo = jv.toObject();
                    if(jo.contains("expand") && jo.value("expand").isArray()) {
                        QString in;
                        QJsonArray expands = jo.value("expand").toArray();
                        for(int i = 0; i < expands.size(); i++) {
                            QJsonObject splitted(jo);
                            QJsonValue ajv = expands.at(i);
                            QStringList keys = QStringList() << "in" << "out" << "comment";
                            foreach(QString key, keys) {
                                if(ajv.isString() && splitted.contains(key) && splitted.value(key).isString()) {
                                    in = splitted.value(key).toString();
                                    in.replace("$${expand}", ajv.toString());
                                    splitted[key] = in;
                                    qDebug() << __FUNCTION__ << "EXPANSION" << key << " --> " << in;
                                }
                            }
                            substs.append(Subst(splitted));
                        }
                    }
                }

                // process input
                //
                foreach (Subst s, substs) {
                    QRegExp inre(s.m_in);
                    int pos;
                    int lineno;
                    qDebug() << __FUNCTION__ << "EXPANSIONL FINDING " << s.m_in << "BAZAN ";
                    pos = inre.indexIn(out);
                    if(pos > -1) {
                        out.replace(inre.cap(1), s.getFirstOutOption());
                        lineno = input.section(inre, 0, 0).count("\n") + 1;
                        m_log.append(OpQuality(s.typeStr(), inre.cap(1), s.getFirstOutOption(), filename(), s.m_comment,
                                               Quality::Ok, lineno));
                    }
                }


            }
        }
    }
    else
        m_msg = "CppInstantiationExpand.process: error opening file " + f.fileName() + " for reading";
    return out;
}

QString CppInstantiationExpand::name()
{
    return "CppInstantiationExpand";
}
