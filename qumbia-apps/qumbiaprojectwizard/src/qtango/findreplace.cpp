#include "findreplace.h"
#include <QTextStream>
#include <QFile>
#include <QtDebug>

#include "conversionhealth.h"
#include <cumbia/cumacros.h>

FindReplace::FindReplace(const QString &id) : FileCmd(id)
{

}

FindReplace::~FindReplace()
{
    pdelete("FindReplace %p", this);
}

QString FindReplace::process(const QString& input)
{
    m_err = false;
    m_msg = "";
    QList<Subst> includes = m_defs.getIncludeSubst();
    QList<Subst> classes = m_defs.getClassSubst();
    QList<ReplaceLine> rep_lines = m_defs.getReplaceLines();
    unsigned lineno = 0;
    QString out;
    m_log.clear();
    bool is_xml = filename().endsWith(".ui") || filename().endsWith(".xml");
    qDebug() << __FUNCTION__ << "filename is  " << filename();

    foreach(QString l, input.split("\n")) {
        int pos = -1;
        QString newline;
        lineno++;
        foreach(Subst su, includes) {
            QString oldinc = su.i_name;
            // find and replace qtango include with cumbia include
            QRegExp oldincre(QString("#include\\s*<(%1)>").arg(oldinc));
            pos = oldincre.indexIn(l);
            if(pos > -1) {
                Quality::Level q = su.quality;
                QString message = su.i_comment;
                out += "// " + l + "\n"; // comment the old line
                if(su.i_inc.length() > 0 && q != Subst::Critical) {
                    newline = "#include <" + su.i_inc + ">";
                    newline += m_comment_add(su.i_comment, is_xml);
                }
                else
                    newline = m_comment_line("no cumbia include replacement found for " + oldincre.cap(1), is_xml);
                // write new include or comment
                out += newline + "\n";
                m_log.append(OpQuality("replace include", oldincre.cap(1),
                                       su.i_inc.length() > 0 ? su.i_inc : "// " + l, filename(), message, q, lineno));
            }
        }

        if(newline.isEmpty()) {
            foreach(Subst su, classes) {
                QString oldclass = su.i_name;
                // find and replace qtango include with cumbia include
                QRegExp oldclre(QString("(%1)\\b").arg(oldclass));
                pos = oldclre.indexIn(l);
                if(pos > -1) {
                    Quality::Level q = su.quality;
                    QString message = su.i_comment;
                    out += m_comment_line(l, is_xml) + "\n"; // comment the old line
                    if(su.i_class.length() > 0 && q != Subst::Critical) {
                        newline = l.replace(oldclre, su.i_class);
                        newline += m_comment_add(su.i_comment, is_xml);
                    }
                    else // leave newline empty so that it's left unchanged. Add a comment
                        out += m_comment_add("no cumbia class replacement found for QTango " + oldclre.cap(1), is_xml) + "\n";

                    // write new line
                    if(su.i_class.length() > 0 && q != Subst::Critical)
                        out += newline + "\n";
                    m_log.append(OpQuality("map class", oldclre.cap(1), su.i_class, filename(), message, q, lineno));
                }
            }
        }
        if(newline.isEmpty()) {
            foreach(ReplaceLine rl, rep_lines) {
                if(filename() == rl.filename || rl.filename == "*") {
                    QRegExp re(rl.regexp);
                    pos = re.indexIn(l);
                    if(pos > -1) {
                        Quality::Level q = rl.quality;
                        QString message = rl.comment;
                        out += m_comment_line(l, is_xml) + "\n"; // comment the old line
                        if(q != Subst::Critical) {
                            newline = rl.replacement;
                            newline += m_comment_add(rl.comment, is_xml);
                        }
                        else // leave newline empty so that it's left unchanged. Add a comment
                            out += m_comment_add("cannot replace QTango specific line: " + re.cap(1), is_xml) + "\n";

                        // write new line
                        if(q != Subst::Critical)
                            out += newline + "\n";
                        m_log.append(OpQuality("replace expr", l, m_comment_line(l, is_xml), filename(), message, q, lineno));
                    }
                }
            }
        }
        if(newline.isEmpty())
            out += l + "\n";
    }
    return out;
}



QString FindReplace::name() {
    return "FindReplace";
}

// add the "after" comment inline. Return the additional comment in line.
// Does not end the line with "\n"
//
QString FindReplace::m_comment_add(const QString &after, bool xml)
{
    QString s;
    if(xml)
        s += "\t<!-- " + after + " ## added by qumbiaprojectwizard -->";
    else
        s+= "//\t" + after + "\t//\t ## added by qumbiaprojectwizard";
    return s;
}

// comments the entire line. No newline is added
QString FindReplace::m_comment_line(const QString &line, bool xml)
{
    if(xml)
        return "<!--  " + line + "  -->";
    else
        return "// " + line;
}
