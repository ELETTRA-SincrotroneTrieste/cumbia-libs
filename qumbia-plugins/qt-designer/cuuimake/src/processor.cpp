#include "processor.h"
#include <QFile>
#include <QDir>
#include <QDateTime>
#include "defs.h"

Processor::Processor()
{
    m_debug = false;
}



void Processor::apply(const Substitutions &subs, const SearchDirInfoSet &dirInfoSet) const
{
    // 1. Find ui files.
    QStringList uifiles;
    QDir wd;

}

QMap<QString, bool> Processor::findUI_H(const SearchDirInfoSet &dirInfoSet)
{
    QDir wd;
    QMap<QString, QDateTime > ui_fstat;
    QMap<QString, QDateTime > ui_h_fstat;
    QMap<QString, bool> fmap;

    // 1. find *.ui files
    QList<SearchDirInfo > dilist = dirInfoSet.getDirInfoList(SearchDirInfoSet::Ui);
    foreach (SearchDirInfo di, dilist ) {
        QString d = di.name();
        if(d != ".")
            wd.cd(d);
        QFileInfoList ui_finfol = wd.entryInfoList(di.filters(), QDir::Files);
        foreach(QFileInfo fi, ui_finfol)
            ui_fstat.insert(fi.fileName(), fi.created());
    }
    // 2. find ui_*.h files
    QDir wd2;
    dilist = dirInfoSet.getDirInfoList(SearchDirInfoSet::Ui_H);
    foreach (SearchDirInfo di, dilist ) {
        QString d = di.name();
        if(d != ".")
            wd2.cd(d);
        QFileInfoList ui_hfinfol = wd2.entryInfoList(di.filters(), QDir::Files);
        foreach(QFileInfo fi, ui_hfinfol)
            ui_h_fstat.insert(fi.fileName(), fi.created());
    }

    // foreach ui file
    foreach(QString uif, ui_fstat.keys())
    {
        QDateTime uif_creat = ui_fstat[uif];
        QString ui_h = "ui_" + uif.remove(".ui") + ".h";
        QDateTime ui_h_fcreat = ui_h_fstat[ui_h];
        if(!ui_h_fcreat.isValid() || uif_creat > ui_h_fcreat)
            fmap.insert(ui_h, true); // need to process
        else
            fmap.insert(ui_h, false); // no need to process
    }
    return fmap;
}
