#include "definitions.h"
#include <QTextStream>
#include <QFile>
#include <QtDebug>

Definitions::Definitions()
{

}

bool Definitions::load(const QString &filename)
{
    QFile f(filename);
    m_err = !f.open(QIODevice::ReadOnly|QIODevice::Text);
    if(!m_err) {
        QTextStream in(&f);
        while(!in.atEnd()) {
            QString line = in.readLine();
            if(!line.contains(QRegExp("^\\s*#.*\\n")) && line.length() > 3)  {
                QStringList parts = line.split(",");
                if(parts.size() == 5 && parts.at(0) == "replace include")
                    m_includes.append(Subst(parts.at(1), parts.at(2), parts.at(3), parts.at(4)));
                else if(parts.size() == 6 && parts.at(0) == "map class")
                    m_classes.append(Subst(parts.at(1), parts.at(2), parts.at(3), parts.at(4), parts.at(5)));
                else if(parts.size() == 6 && parts.at(0) == "replace expr") {
                    m_replacelines.append(ReplaceLine(parts.at(1), parts.at(2), parts.at(3), parts.at(4), parts.at(5)));
                }
                else
                    printf("\e[1;31mConversionDefs.load: ignoring line \"%s\"\e[0m\n", line.toStdString().c_str());
            }
        }
        f.close();
        return true;
    }
    else {
        m_msg = "ConversionDefs.load: error opening file " + filename + " in read mode: " + f.errorString();
    }
    return !m_err;
}
