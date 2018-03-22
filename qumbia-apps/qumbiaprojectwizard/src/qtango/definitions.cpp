#include "definitions.h"
#include <QTextStream>
#include <QFile>
#include <QtDebug>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonArray>
#include <QJsonValue>

Definitions::Definitions()
{

}

bool Definitions::load(const QString &filename)
{
    // block
    // ([A-Za-z0-9_\s]+)\n*\s*\{([A-Za-z0-9_\.,\[\]:\s/><;'+=\-'%\(\)\*\^\$~!`]+)\n*\s*\}
    QFile f(filename);
    m_err = !f.open(QIODevice::ReadOnly|QIODevice::Text);
    if(!m_err) {
        QByteArray content = f.readAll();
        QJsonParseError pe;
        QJsonDocument jd = QJsonDocument::fromJson(content, &pe);
        m_err = (pe.error != QJsonParseError::NoError);
        if(m_err)
            m_msg = pe.errorString() + ": offset: " + QString::number(pe.offset);
        else {
            QJsonValue root = jd.object().value("array");
            m_err = root.isUndefined() || root.isNull() || !root.isArray();
            if(!m_err) {
                QJsonArray array = root.toArray();
                for(int i = 0; i < array.size(); i++) {
                    QJsonValue jv = array.at(i);
                    QJsonObject jo = jv.toObject();
                    m_substs.append(Subst(jo));
                }
            }
            else {
                m_msg = "root element \"array\" in " + f.fileName() + " does not contain an array";
            }
        }
        f.close();
        return !m_err;
    }
    else {
        m_msg = "ConversionDefs.load: error opening file " + filename + " in read mode: " + f.errorString();
    }
    return !m_err;
}
