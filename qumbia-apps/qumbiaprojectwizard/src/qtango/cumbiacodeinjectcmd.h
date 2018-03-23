#ifndef CUMBIACODEADDCMD_H
#define CUMBIACODEADDCMD_H

#include "fileprocessor_a.h"
#include <QString>
#include <QList>

class Section {
public:
    enum Where { NoSection = -1, StartOfCppConstructor, EndOfCppConstructor,
                 StartOfHConstructor, EndOfHConstructor,
                 MainCppBeforeNewWidget, EndOfMain, Includes, ElseWhere };

    Section() {
        where = NoSection;
    }

    Section(const QString& txt, Where w, const QString& betw_re = QString()) {
        text = txt;
        where = w;
        between_regexp = betw_re;
    }

    bool isEmpty() const { return text.length() == 0; }

    QString text;

    // if Where is Elsewhere, tells the code injector to inject
    // the section within the regexp, replacing "%1"
    //
    QString between_regexp;

    QString whereStr() const {
        switch (where) {
        case NoSection:
            return "no section";
        case StartOfCppConstructor:
            return "cpp constructor: start";
        case EndOfCppConstructor:
            return "cpp constructor: end";
        case StartOfHConstructor:
            return "class def: start";
        case EndOfHConstructor:
            return "class def: end";
        case MainCppBeforeNewWidget:
            return "main.cpp";
        case EndOfMain:
            return "main.cpp: end";
        case  Includes:
            return "include files";
        case ElseWhere:
            return "elsewhere";
        default:
            break;
        }
    }

    Where where;

};

class CodeExtractorA
{
public:
    enum Type { MainCpp, MainWCpp, MainWH, MainWUi, Pro };

    virtual Type type() const = 0;

    virtual QList<Section> extract(CodeExtractorA::Type t) = 0;

    bool error() const { return m_err; }

    QString errorMessage() const { return m_msg; }

protected:

    bool m_err;
    QString m_msg;
    QString m_get_file_contents(CodeExtractorA::Type t);
};


class CumbiaCodeInjectCmd : public FileCmd
{

public:
    CumbiaCodeInjectCmd(const QString& filename, const QString& mainwidgetclass, const QString& mainwidgetvar, const QString &uiformclass);
    // FileCmd interface
    virtual QString process(const QString &input);
    virtual QString name();

private:
    QString m_mainwidgetclass, m_mainwidgetvar, m_uiformclass;

    QString m_get_file_contents(CodeExtractorA::Type t);

};

#endif // CUMBIACODEADDCMD_H
