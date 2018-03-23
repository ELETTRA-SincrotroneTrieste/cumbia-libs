#include "findreplace.h"
#include <QTextStream>
#include <QFile>
#include <QtDebug>

#include <QDialog>
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QMessageBox>
#include <QTextEdit>

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
    QList<Subst> subs = m_defs.getSubstitutions();
    unsigned lineno = 0;
    QString out;
    m_log.clear();
    bool is_xml = filename().endsWith(".ui") || filename().endsWith(".xml");

    foreach(const QString& l, input.split("\n")) {
        QString oldline(l);
        int pos = -1;
        QString newline, replace;
        lineno++;
        // skip commented lines \s*<!\-\-.*\-\->|\n\s*/\*.*\*/\s*|\s*//.*
        QRegExp commentRe("\\s*<!\\-\\-.*\\-\\->|\\n\\s*/\\*.*\\*/\\s*|\\s*//.*");

        if(commentRe.indexIn(l) >= 0)
            continue;

        foreach(Subst su, subs) {
            if(su.m_type == Subst::ReplaceInclude) {
                QString oldinc = su.m_in;
                // find and replace qtango include with cumbia include
                QRegExp oldincre;
                if(!is_xml)
                    oldincre.setPattern(QString("#include\\s*<(%1)>").arg(oldinc));
                else
                    oldincre.setPattern(QString("<header>(%1)</header>").arg(oldinc));
                pos = oldincre.indexIn(l);
                if(pos > -1) {
                    Quality::Level q = su.quality;
                    QString message = su.m_comment;
                    out +=  m_comment_line(l, is_xml) + "\n"; // comment the old line
                    // if there is more than one possible out, let the user
                    // select one
                    replace = get_option(su.m_comment,  l, su.m_out_options);
                    if(!replace.isEmpty() && q != Subst::Critical) {
                        if(!is_xml)
                            newline = "#include <" + replace + ">";
                        else
                            newline = "<header>" + replace + "</header>";
                       newline += m_comment_add(su.m_comment, is_xml);
                    }
                    else
                        newline = m_comment_line("no cumbia include replacement found for " + oldincre.cap(1), is_xml);
                    // write new include or comment
                    out += newline + "\n";
                    m_log.append(OpQuality("replace include", oldincre.cap(0),
                                           newline, filename(), message, q, lineno));
                }
            }
            else if(su.m_type == Subst::MapClass) {
                QString oldclass = su.m_in;
                // find and replace qtango include with cumbia include
                QRegExp oldclre(QString("(%1)\\b").arg(oldclass));
                pos = oldclre.indexIn(l);
                if(pos > -1) {
                    Quality::Level q = su.quality;
                    QString message = su.m_comment;
                    out += m_comment_line(l, is_xml) + "\n"; // comment the old line
                    // if there is more than one possible out, let the user
                    // select one
                    replace = get_option(su.m_comment, l, su.m_out_options);
                    if(replace.length() > 0 && q != Subst::Critical) {
                        newline = oldline.replace(oldclre, replace);
                        newline += m_comment_add(su.m_comment, is_xml);
                    }
                    else // leave newline empty so that it's left unchanged. Add a comment
                        out += m_comment_add("no cumbia class replacement found for QTango " + oldclre.cap(1), is_xml) + "\n";

                    // write new line
                    if(su.m_include.length() > 0 && q != Subst::Critical)
                        out += newline + "\n";
                    m_log.append(OpQuality("map class", oldclre.cap(1), replace, filename(), message, q, lineno));
                }
            }
            else if(su.m_type == Subst::ReplaceLine || su.m_type == Subst::ReplaceExpr) {
                if(filename() == su.m_file || su.m_file == "*") {
                    QRegExp re(su.m_in);
                    Quality::Level q = su.quality;
                    QString message = su.m_comment;
                    pos = re.indexIn(l);
                    if(pos > -1) {
                        out += m_comment_line(l, is_xml) + "\n"; // comment the old line
                        replace = get_option(su.m_comment, l, su.m_out_options);
                        if(q != Subst::Critical && su.m_type == Subst::ReplaceLine)
                            newline = replace;
                        else if(q != Subst::Critical && su.m_type == Subst::ReplaceExpr) {
                            newline = oldline.replace(re.cap(1), replace);
                        }
                        if(q != Subst::Critical)
                            newline += m_comment_add(su.m_comment, is_xml);

                        // write new line
                        if(q != Subst::Critical) {
                            out += newline + "\n";
                            m_log.append(OpQuality(su.typeStr(), l, newline, filename(), message, q, lineno));
                        }
                        else { // leave newline empty so that it's left unchanged. Add a comment
                            out += m_comment_add("cannot replace QTango specific line: " + re.cap(1), is_xml) + "\n";
                            m_log.append(OpQuality(su.typeStr(), l, newline, filename(), message, q, lineno));
                        }
                    }

                }
            }
            if(pos >= 0) // a match has been found for the line, stop processing that line
                break;
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

QString FindReplace::get_option(const QString &text, const QString& snippet, const QStringList &options)
{
    QString o;
    if(options.size() == 1)
        o = options.first();
    else {
        GetOptionDialog dlg(text, snippet, options, 0);
        dlg.exec();
        o = dlg.m_selectedOption;
    }
    return o;
}

// comments the entire line. No newline is added
QString FindReplace::m_comment_line(const QString &line, bool xml)
{
    if(xml)
        return "<!--  " + line + "  -->";
    else
        return "// " + line;
}

GetOptionDialog::GetOptionDialog(const QString& comment, const QString& line, const QStringList& options, QWidget *parent) : QDialog(parent)
{
    setWindowTitle("select an option");
    QGridLayout *lo = new QGridLayout(this);
    QLabel *l = new QLabel("Multiple options available: you must select one:", this);
    QComboBox *cb = new QComboBox(this);
    cb->insertItems(0, options);
    QTextEdit *tecomment = new QTextEdit(this);
    tecomment->setText(comment);
    tecomment->setReadOnly(true);
    QLineEdit *lesnippet = new QLineEdit(line, this);
    lesnippet->setReadOnly(true);
    QPushButton *pb = new QPushButton("Choose", this);
    lo->addWidget(l, 0, 0, 1, 4);
    lo->addWidget(cb, 1, 0, 1, 3);
    lo->addWidget(pb, 1, 3, 1, 1);
    lo->addWidget(lesnippet, 2, 0, 1, 4);
    lo->addWidget(tecomment, 3, 0, 4, 4);
    connect(pb, SIGNAL(clicked()), this, SLOT(accept()));
}

int GetOptionDialog::exec()
{
    int ret = QDialog::exec();
    if(ret == QDialog::Accepted)
        m_selectedOption = findChild<QComboBox *>()->currentText();
    return ret;
}

void GetOptionDialog::reject()
{
    QMessageBox::information(this, "No selection available", "You did not select an option\n"
                                                             "The line will be commented.");
}
