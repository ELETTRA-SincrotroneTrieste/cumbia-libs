#include "parser.h"
#include "defs.h"
#include <QDir>
#include <QFile>
#include <QList>
#include <QTextStream>
#include <QRegExp>
#include <QtDebug>

Parser::Parser(const QString& force_mode)
{
    m_error = false;
    m_forceMode = force_mode;
    m_debug = false;
}

QString Parser::detect(const SearchDirInfoSet &dirInfoSet, const QList<Search> &searchlist, Substitutions &subs)
{
    QString mode;
    bool error = false;
    m_error = false;
    m_lastError = "";
    if(dirInfoSet.isEmpty())
        m_lastError = "Parser::detect: error: directory search list is empty";
    if(searchlist.isEmpty())
        m_lastError = "Parser::detect: list of search modes is empty";
    m_error = !m_lastError.isEmpty();
    if(m_error)
        return "";

    int i = 0;
    foreach(Search se, searchlist)
    {
        i++;
        if(m_debug)
            printf(" searching mode %d/%d: \"%s\" \n", i, searchlist.size(), qstoc(se.factory));

        foreach(SearchDirInfo dirInfo, dirInfoSet.getDirInfoList(SearchDirInfoSet::Source))
        {
            QDir wdir;
            QString dirnam = dirInfo.name();
            if(dirnam != ".")
                wdir.cd(dirnam);
            QStringList files = wdir.entryList(dirInfo.filters(), QDir::Files|QDir::Readable);
            foreach(QString fnam, files)
            {
                if(m_debug)
                    printf("Parser::detect: \e[1;32manalyzing file \"%s\"\e[0m under dir \"%s\"\n", qstoc(fnam), qstoc(dirnam));
                fnam = dirnam + "/" + fnam;
                QFile file(fnam);
                error = !file.open(QIODevice::ReadOnly | QIODevice::Text);
                m_error |= error;
                if(error) {
                    m_lastError = "Parser::detect: error opening \"" +fnam + "\" in dir \"" +
                            wdir.absolutePath() + "\" in read mode: " + file.errorString() + ": skipping";
                    if(m_debug)
                        printf("%s\n", qstoc(m_lastError));
                }
                else
                {
                    bool contains = false;
                    QTextStream in(&file);
                    QString f = in.readAll();
                    QString varname;
                    {
                        foreach(QString classname, se.searches)
                        {
                            contains = find_match(f, classname, varname);

                            if(contains)
                            {
                                if(m_debug) printf(" \e[1;32mfound declaration \"%s\" var name \"%s\"\e[0m\n", qstoc(classname), qstoc(varname));
                                subs.insertSubstitution(classname, varname);
                                subs.selectMode(se.factory);
                            }
                            else if(m_debug)
                                printf(" declaration for \"%s\" not found\n", qstoc(classname));

                        }
                        if(contains && mode.isEmpty())
                        {
                            mode = se.factory;
                        }
                        else if(contains && !mode.isEmpty())
                        {
                            m_error = true;
                            m_lastError = "ModeDetector.detect: conflicting modes detected: " + se.factory + " and " + mode;
                            return QString();
                        }
                        if(m_debug && contains)
                            printf("ModeDetector.detect: file %s indicates mode \"%s\" is in use\n", qstoc(fnam), qstoc(mode));
                        else if(m_debug)
                            printf("ModeDetector.detect: file %s is not helpful to detect mode\n", qstoc(fnam));
                    }

                    file.close();
                }

            }
            if(m_debug)
                printf("Parser.detect: substitutions: %s\n", qstoc(subs.toString()));
        }
        // if we have found matches for a mode (CumbiaTango, Cumbia, CumbiaPool), then skip the other modes
        // Modes in searchlist are ordered according to the nodes order in the xml file.
        if(!subs.selectedMode().isEmpty())
        {
            if(m_debug)
                printf("Parser.detect: skipping next modes because \e[1;32m\"%s\" has been detected\e[0m\n", qstoc(subs.selectedMode()));
            break;
        }
    }
    return subs.selectedMode();
}

bool Parser::error() const
{
    return m_error;
}

QString Parser::lastError() const
{
    return m_lastError;
}

void Parser::setDebug(bool dbg)
{
    m_debug = dbg;
}

const char *Parser::tos(const QString &s)
{
    return s.toStdString().c_str();
}

bool Parser::find_match(const QString &contents, const QString &classname, QString &varname) const
{
    varname = "";
    int index = 1;
    // first version with "new" find varname = new classname(params...);
    // ([A-Za-z_0-9]*)\s*=\s*new\s+(CumbiaPool)\s*\(.*\);
    QRegExp re;
    if(classname.contains(QRegExp("[A-Za-z_0-9]*\\s*\\*")))
        re.setPattern(QString("([A-Za-z_0-9]*)\\s*=\\s*new\\s+(%1)\\s*\\(.*\\);").arg(classname));
    else // second version: stack declaration
    {
        re.setPattern(QString("(%1)\\s+([A-Za-z_0-9]*);").arg(classname));
        index = 2;
    }

    re.setMinimal(true);
    if(contents.contains(re))
    {
        int pos = re.indexIn(contents);
        if(pos > -1 && re.capturedTexts().count() == 3)
        {
           // qDebug() << __FUNCTION__ << "CAPTURED" << re.capturedTexts();
            varname = re.capturedTexts().at(index);
        }
    }
    return varname != QString();
}
