#include "prosectionextractor.h"
#include <QFile>
#include <QTextStream>
#include <QRegExp>

// include _ and - in file name
// (\bFORMS\s*[\+]{0,1}=\s*[a-zA-Z0-9_\-/\s\\\.\$#]*)(\n*$|\n\b[A-Za-z0-9]*)
ProSectionExtractor::ProSectionExtractor(const QString &path)
{
    QFile f(path);
    QString s;
    m_err = !f.open(QIODevice::ReadOnly|QIODevice::Text);
    if(!m_err) {
        QTextStream in(&f);
        s = in.readAll();
        f.close();
    }
    else
        m_msg = "ProSectionExtractor: error opening file " + f.fileName() + ": " + f.errorString();
    if(!m_err)
    {
        foreach(QString r, QStringList() << "SOURCES" << "HEADERS" << "FORMS") {
            QRegExp re(QString("(\\b%1\\s*[\\+]{0,1}=\\s*[a-zA-Z0-9_\\-/\\s\\\\\\.\\$#]*)(\\n*$|\\n\\b[A-Za-z0-9]*)").arg(r));
            int pos = re.indexIn(s);
            if(pos > -1 && re.cap(1).length() > 0) {
                m_sections[r] = re.cap(1);
            }
        }
    }
}

QString ProSectionExtractor::get(const QString &section)
{
    return m_sections.value(section);
}
