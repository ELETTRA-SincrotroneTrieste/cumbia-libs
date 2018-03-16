#include "proconvertcmd.h"
#include <QFile>
#include <QTextStream>
#include <QtDebug>

ProConvertCmd::ProConvertCmd(const QString& filenam) : FileCmd(filenam)
{

}

/*
 * Use the qumbiaproject-tango.pro template and replace
 * HEADERS, SOURCES, FORMS, TARGET from the input pro file.
 * FORMS are commented.
 *
 * regexp to match SOURCES/HEADERS,...
 * The second () matches either optional newline and end of string if input
 * ends with SOURCES or newline (without \) plus a new word
 * \$ is in the pattern because present in template
 * if commented lines are present in the SOURCES/HEADERS list then this does not work
 * (\bSOURCES\s*[\+]*=\s*[a-zA-Z0-9/\s\\\.\$_]*)(\n|\n*$|\n\b[A-Za-z0-9]*)
 */
QString ProConvertCmd::process(const QString &input)
{
    QString rep;
    QString pro_template_fnam = QString(TEMPLATES_PATH) + "/qumbiaproject-tango.pro";
    QString pro_out = m_get_file_contents(pro_template_fnam);
    pro_out.replace("$INCLUDE_DIR$", INCLUDE_PATH);
    int in_pos, out_pos;
    QStringList replacements =
            QStringList() << "SOURCES" << "HEADERS"  << "TARGET" << "FORMS";
    foreach(QString r, replacements) {
        QRegExp in_re(QString("(\\b%1\\s*[\\+]*=\\s*[a-zA-Z0-9/\\s\\\\\\.\\$_]*)(\\n|\\n*$|\\n\\b[A-Za-z0-9]*)").arg(r));
        QRegExp template_re;
        // template has commented FORMS section: # FORMS    = src/$FORMFILE$
        if(r == "FORMS")
            template_re.setPattern("(# FORMS\\s*[\\+]*=\\s*src/\\$FORMFILE\\$)");
        else
            template_re.setPattern(in_re.pattern());
        in_pos = in_re.indexIn(input);
        out_pos = template_re.indexIn(pro_out);
        int lineno = pro_out.section(in_re, 0, 0).count("\n") + 1;

        qDebug() << __FUNCTION__ << "replacing section " << input << "\n\n\n" << pro_out << "\n\n";
        qDebug() << __FUNCTION__ << "replacing section " << r << in_pos << out_pos;
        m_err = (in_pos < 0 || out_pos < 0);
        if(m_err) {
            rep = in_re.cap(1);
            if(r == "FORMS")
                rep = m_comment_lines(rep);
            pro_out.replace(template_re.cap(1), rep);
            m_log.append(OpQuality("pro file", r, r == "FORMS" ? "# " + r : r, filename(), "replaced", Quality::Ok, lineno));
        }
        if (in_pos < 0)
            m_msg = "ProConvertCmd.process: input pro file " + filename() + " does not contain section " + r;
        else if(out_pos < 0)
            m_msg = "ProConvertCmd.process: template pro file " + filename() + " does not contain section " + r +
                    "\nContact the developer";
    }

    return pro_out;
}

QString ProConvertCmd::name()
{
    return "ProConvertCmd";
}

QString ProConvertCmd::m_get_file_contents(const QString& fnam)
{
    QString s;
    QFile f(fnam);
    m_err = !f.open(QIODevice::ReadOnly|QIODevice::Text);
    if(!m_err) {
        QTextStream in(&f);
        s = in.readAll();
        f.close();
    }
    else
        m_msg = "CumbiaCodeInjectCmd::m_get_file_contents: error opening template file " + f.fileName() + ": " + f.errorString();

    return s;
}

QString ProConvertCmd::m_comment_lines(const QString &s)
{
    QString out;
    QStringList lines = s.split("\n");
    foreach(QString l, lines) {
        out += "#\t" + l + "\n";
    }
    return out;
}

QString ProConvertCmd::m_remove_comments(const QString &s)
{
    QString out;
    QRegExp commentRe("^\\s*(#)+");
    foreach(QString line, s.split("\n")) {
        commentRe.indexIn(line);
        if(commentRe.cap(1).isEmpty())
            out += line + "\n";
    }
    return out;
}
