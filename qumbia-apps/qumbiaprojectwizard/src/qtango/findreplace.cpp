#include "findreplace.h"
#include <QTextStream>
#include <QFile>
#include <QtDebug>

#include "conversionhealth.h"

FindReplace::FindReplace()
{

}

bool FindReplace::load(const QString &filename)
{
    QFile f(filename);
    m_err = !f.open(QIODevice::ReadOnly|QIODevice::Text);
    if(!m_err) {
        QTextStream in(&f);
        while(!in.atEnd()) {
            QString line = in.readLine();
            if(!line.contains(QRegExp("^\\s*#.*\\n")) && line.length() > 3)  {
                QStringList parts = line.split(",");
                if(parts.size() == 5 && parts.at(0) == "include")
                    includes.append(Subst(parts.at(1), parts.at(2), parts.at(3), parts.at(4)));
                else if(parts.size() == 6 && parts.at(0) == "map")
                    classes.append(Subst(parts.at(1), parts.at(2), parts.at(3), parts.at(4), parts.at(5)));
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

QString FindReplace::replace(const QString &filename)
{
    unsigned lineno = 0;
    QString out;
    QFile f(filename);
    m_log.clear();
    m_err = !f.open(QIODevice::ReadOnly|QIODevice::Text);
    if(!m_err) {
        QString l;
        QTextStream in(&f);
        while(!in.atEnd()) {
            int pos = -1;
            QString newline;
            l = in.readLine();
            lineno++;
            foreach(Subst su, includes) {
                QString oldinc = su.i_name;
                // find and replace qtango include with cumbia include
                QRegExp oldincre(QString("#include\\s*<(%1)>").arg(oldinc));
                pos = oldincre.indexIn(l);
                if(pos > -1) {
                    Subst::Quality q = su.quality;
                    QString message = su.i_comment;
                    out += "// " + l + "\n"; // comment the old line
                    if(su.i_inc.length() > 0 && q != Subst::Critical)
                        newline = "#include <" + su.i_inc + "> // " + su.i_comment;
                    else
                        newline = "// no cumbia include replacement found for " + oldincre.cap(1);
                    // write new include or comment
                    out += newline + "\n";
                    m_log.append(ReplaceQuality("include", oldincre.cap(1), su.i_inc, message, q, lineno));
                }
            }

            if(newline.isEmpty()) {
                foreach(Subst su, classes) {
                    QString oldclass = su.i_name;
                    // find and replace qtango include with cumbia include
                    QRegExp oldclre(QString("(%1)\\b").arg(oldclass));
                    pos = oldclre.indexIn(l);
                    if(pos > -1) {
                        Subst::Quality q = su.quality;
                        QString message = su.i_comment;
                        out += "// " + l + "\n"; // comment the old line
                        if(su.i_class.length() > 0 && q != Subst::Critical)
                            newline = l.replace(oldclre, su.i_class)  + " // " + su.i_comment;
                        else // leave newline empty so that it's left unchanged. Add a comment
                            out += "// no cumbia class replacement found for QTango " + oldclre.cap(1);

                        // write new line
                        if(su.i_class.length() > 0 && q != Subst::Critical)
                            out += newline + "\n";
                        m_log.append(ReplaceQuality("class", oldclre.cap(1), su.i_class, message, q, lineno));
                    }
                }
            }
            if(newline.isEmpty())
                out += l + "\n";
        }
    }
    else {
        m_msg = "ConversionDefs.convert: error opening file " + filename + " in read mode: " + f.errorString();
    }

    QFile outf("/tmp/main_out.cpp");
    if(outf.open(QIODevice::WriteOnly)) {
        QTextStream outs(&outf);
        outs << out;
        qDebug() << __FUNCTION__ << "written output to " << outf.fileName();
        outf.close();
    }
    return out;
}
