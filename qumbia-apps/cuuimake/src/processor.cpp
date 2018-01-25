#include "processor.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDateTime>
#include <QCoreApplication>
#include <QDebug>
#include "defs.h"

Processor::Processor()
{
    m_debug = false;
    m_error = false;
}

bool Processor::expand(const Substitutions& subs, const QMap<QString,
                       Expand>& objectExpMap, const QString& ui_h_fname,
                       const SearchDirInfoSet &dirInfoSet)
{
    QString mode = subs.selectedMode();
    QDir wd;
    bool ok = true;


    bool file_exists = wd.exists(ui_h_fname);

    QString filename = m_getUiHFileName(ui_h_fname, dirInfoSet);
    if(filename.isEmpty()) // m_getUiHFileName sets m_lastError
        return false;

    QFile f(filename);
    m_error = !f.open(QIODevice::ReadOnly | QIODevice::Text);
    if(m_error)
    {
        m_lastError = "Processor::expand: error opening file \"" + ui_h_fname +
                "\" in read mode under \"" + wd.absolutePath() + "\": " + f.errorString();
        return false;
    }

    QTextStream in(&f);
    int pos;
    QStringList list;
    QString line;
    QString expanded_ui_h;
    QString expanded_params;
    QString replacement;
    QString par;
    QString class_name;
    QString comment;
    int i;
    while (!in.atEnd())
    {
        // The returned line has no trailing end-of-line characters ("\n" or "\r\n")
        line = in.readLine();
        foreach(QString objectnam, objectExpMap.keys())
        {
            qDebug() << __FUNCTION__ << "PRICESSING" << objectnam;
            bool isSetupUi = false;
            // match and capture "new QuLabel(parentname);" \s*=\s*new\s+QuLabel\(([A-Za-z_0-9]+)\)
            QRegExp re(QString("\\s*=\\s*new\\s+%1\\(([A-Za-z_0-9]+)\\);").arg(objectnam));
            re.setMinimal(true);
            pos = re.indexIn(line);
            if(pos < 0)
            {
                // expand setupUi
                re.setPattern(QString("\\s*void\\s*%1\\(([A-Za-z_0-9\\s\\*]*)\\)").arg(objectnam));
                pos = re.indexIn(line);
                isSetupUi = pos > -1;
                if(pos > -1)
                    qDebug() << __FUNCTION__ << "DETECTED setupUI regex";
            }

            if( pos > -1 && !line.contains("//") )
            {
                qDebug() << __FUNCTION__ << "PRECEISSING" << line;
                list = re.capturedTexts();
                if(list.size() == 2) // full match [0] and capture [1]
                {
                    expanded_params = list.at(1);
                    // get Expand from object name
                    const Expand& expand = objectExpMap.value(objectnam);
                    // expand.params is of type Params
                    // params.map is a QMap<QString, QStringList> where the key is the mode (factory)
                    // and the values are the ordered list of the class names
                    const QList<Par> & parlist = expand.params.getParList(mode);
                    // with the class names, get the substitutions in the correct order
                    qDebug() << __FUNCTION__ << list << objectnam;
                    for(i = 0; i< parlist.size() && !m_error; i++)
                    {
                        QString pardef;
                        Par p = parlist[i];
                        qDebug() << __FUNCTION__ << objectnam << p.toString() << isSetupUi;
                        class_name = p.classname;
                        pardef = p.pardef;
                        par = subs.getSubstitution(class_name);
                        m_error = (par.isEmpty());
                        qDebug() << __FUNCTION__ << class_name << "parameter def " << pardef << isSetupUi;
                        expanded_params += ", " + pardef + par;
                    }
                    if(m_error)
                    {
                        m_lastError = "Processor::expand: error: no substitutions found for class name \"" + class_name + "\"." +
                                " File \"" + ui_h_fname + "\" left unchanged.";
                        return false;
                    }
                    if(parlist.size() > 0)
                    {
                        comment = line + " // expanded by " + qApp->applicationName() + " v." + qApp->applicationVersion();
                        expanded_ui_h += "// " + comment + "\n";
                        line = line.replace(list.at(1), expanded_params);
                        expanded_ui_h += line + "\n";
                        qDebug() << __FUNCTION__ << "BREAKING CUZ PARLIST SIZE > 0";
                        break; // go to next line
                    }
                }
                else
                    qDebug() << __FUNCTION__ << "regexp matches " << list.size();
            }
        }
        if(pos < 0)
            expanded_ui_h += line + "\n";

    } // while (!in.atEnd)

    f.close();

    m_error = !f.open(QIODevice::WriteOnly | QIODevice::Text);
    if(m_error)
    {
        m_lastError = "Processor::expand: error opening file \"" + ui_h_fname + "\" under \"" +
                wd.absolutePath() + "\" in write mode: " + f.errorString();
        return false;
    }

    QTextStream out(&f);
    out << expanded_ui_h;
    f.close();

    return true;

}

QMap<QString, bool> Processor::findUI_H(const SearchDirInfoSet &dirInfoSet)
{
    QDir wd;
    QMap<QString, QDateTime > ui_h_fstat;
    QMap<QString, bool> fmap;
    QDateTime newest_h_cpp_ui_creat, oldest_ui_h_creat;
    QStringList ui_files, ui_h_files;
    // 1. find *.ui files, *.cpp and *.h files. Also .h and .cpp files must be taken
    //    into account because the CumbiaPool, CumbiaTango, CuControlsFactoryPool variable
    //    names can be changed
    QList<SearchDirInfo > di_ui_list = dirInfoSet.getDirInfoList(SearchDirInfoSet::Ui);
    QList<SearchDirInfo > di_h_cpp_list = dirInfoSet.getDirInfoList(SearchDirInfoSet::Source);
    QList<SearchDirInfo> di_list = di_ui_list + di_h_cpp_list;
    foreach (SearchDirInfo di, di_list )
    {
        QString d = di.name();
        if(d != ".")
            wd.cd(d);
        QFileInfoList finfol = wd.entryInfoList(di.filters(), QDir::Files);
        foreach(QFileInfo fi, finfol)
        {
            if(!newest_h_cpp_ui_creat.isValid() || fi.created() > newest_h_cpp_ui_creat)
            {
                newest_h_cpp_ui_creat = fi.created();
          //      qDebug() << __FUNCTION__ << "newest file among h cpp ui is " << fi.fileName() << " date time " << newest_h_cpp_ui_creat;
            }
            if(fi.fileName().endsWith(".ui"))
                ui_files << fi.fileName();
        }
    }
    // 2. find ui_*.h files
    QDir wd2;
    di_ui_list = dirInfoSet.getDirInfoList(SearchDirInfoSet::Ui_H);
    foreach (SearchDirInfo di, di_ui_list ) {
        QString d = di.name();
        if(d != ".")
            wd2.cd(d);
        QFileInfoList ui_hfinfol = wd2.entryInfoList(di.filters(), QDir::Files);
        foreach(QFileInfo fi, ui_hfinfol)
        {
            if(fi.fileName().contains(QRegExp("ui_[A-Za-z_0-9]+\\.h")))
            {
                ui_h_files << fi.fileName();
                if(!oldest_ui_h_creat.isValid() || fi.created() < oldest_ui_h_creat)
                {
                    oldest_ui_h_creat = fi.created();
                //    qDebug() << __FUNCTION__ << "oldst file ui_*h is " << fi.fileName() << " date time " << oldest_ui_h_creat;
                }
            }
        }
    }

    // foreach ui file
    foreach(QString ui_file, ui_files)
    {
        QString ui_h = "ui_" + ui_file.remove(".ui") + ".h";
        // if the newest h, ui or cpp file is more recent than the oldest ui_*.h file, then mark
        // the ui_*.h file for expansion. newest_h_cpp_ui_creat.isValid() is to guarantee that at
        // least one cpp, h or ui file has been found.
        if(oldest_ui_h_creat < newest_h_cpp_ui_creat || !ui_h_files.contains(ui_h))
            fmap.insert(ui_h, newest_h_cpp_ui_creat.isValid()); // need to process
        else
            fmap.insert(ui_h, false); // no need to process
    }
    return fmap;
}

int Processor::remove_UI_H(const SearchDirInfoSet &dirInfoSet)
{
    int remcnt = 0;
    QList<SearchDirInfo > di_ui_h_list = dirInfoSet.getDirInfoList(SearchDirInfoSet::Ui_H);
    // find ui_*.h files
    QDir wd2;
    foreach (SearchDirInfo di, di_ui_h_list ) {
        QString d = di.name();
        if(d != ".")
            wd2.cd(d);
        QFileInfoList ui_hfinfol = wd2.entryInfoList(di.filters(), QDir::Files);
        foreach(QFileInfo fi, ui_hfinfol)
        {
            if(fi.fileName().contains(QRegExp("ui_[A-Za-z_0-9]+\\.h")))
            {
                bool ok = wd2.remove(fi.fileName());
                if(ok) remcnt++;
            }
         }
    }
    return remcnt;
}

QString Processor::m_getUiHFileName(const QString& ui_h_fname,
                                    const SearchDirInfoSet &dirInfoSet)
{
    // 1. Find ui files.
    QDir wd;
    QList<SearchDirInfo> dilist = dirInfoSet.getDirInfoList(SearchDirInfoSet::Ui_H);
    m_error = dilist.size() != 1;
    if(m_error)
    {
        m_lastError = "Processor.expand: number of directories defined to generate ui files is not one";
        return "";
    }
    bool ok = true;
    QString dnam = dilist.at(0).name();
    if(dnam != ".")
    {
        if(!wd.cd(dnam))
        {
            ok = wd.mkdir(dnam);
            if(ok)
            {
                qDebug() << __FUNCTION__ << "changing into " << dnam;
                wd.cd(dnam);
            }
        }
    }
    if(ok)
        ok = wd.exists(ui_h_fname);
    if(ok)
        return  dnam + "/" + ui_h_fname;
    return "";
}
