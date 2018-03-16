#include "projectbackup.h"
#include <QDir>
#include <QString>
#include <QMessageBox>
#include <QFileInfoList>
#include <QtDebug>

ProjectBackup::ProjectBackup(const QString &dir)
{
    m_dirnam = dir;
}

ProjectBackup::Result ProjectBackup::popup_message() const
{
    QString title, message;
    ProjectBackup::Result res;
    QDir dir(m_dirnam);
    if(dir.exists("qtango_backup") && !dir.isEmpty() ) {
        title = "QTango backup already found";
        message = "A folder named \"qtango_backup\""
                  "has already been found in the output directory\n"
                  "The files in the output directory will not be backed up "
                  "while the\n\"qtango_backup\" folder is there.";
        res = ProjectBackup::BackupUnnecessary;
    }
    else {
        title = "QTango input and cumbia output project directories coincide";
        message = "Do you want to proceed? If yes, a \"qtango_backup\" directory\n"
                  "will be created and will contain the full backup of the project";
        res = ProjectBackup::NeedsBackup;
    }

    int ret = QMessageBox::information(0, title, message, QMessageBox::Yes|QMessageBox::No);
    if(ret != QMessageBox::Yes)
        return Cancel;
    return res;
}

ProjectBackup::Result ProjectBackup::save(const QString &backupdir) const
{
    Result result = BackupFailed;
    QDir d(m_dirnam);
    if(d.exists() && d.cd(".")) {
        if(d.mkdir(backupdir)) {
            bool success = m_copyRecursively(m_dirnam, m_dirnam + "/" + backupdir);
            if(success)
                result = BackedUp;
        }
        else if(d.exists(backupdir)) {
            result = BackupUnnecessary;
        }
    }
    return result;
}

bool ProjectBackup::m_copyRecursively(QString from_dir, QString to_dir, bool replace_on_conflict) const
{
    QDir dir;
    dir.setPath(from_dir);
    QStringList exclude = QStringList() << "CVS" << ".git" << ".svn";
    from_dir += QDir::separator();
    to_dir += QDir::separator();

    foreach (QString copy_file, dir.entryList(QDir::Files))
    {
        QFile cf(copy_file);
        if(exclude.contains(cf.fileName()))
            continue;
        QString from = from_dir + copy_file;
        QString to = to_dir + copy_file;

        if (QFile::exists(to)) {
            if (replace_on_conflict && QFile::remove(to) == false)
                return false;
            else
                continue;
        }

        if (QFile::copy(from, to) == false) {
            return false;
        }
    }

    foreach (QString copy_dir, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
        QString from = from_dir + copy_dir;
        QString to = to_dir + copy_dir;

        if(QDir(to_dir).dirName() == copy_dir || exclude.contains(copy_dir))
            continue;
        if (dir.mkpath(to) == false)
            return false;

        if (m_copyRecursively(from, to, replace_on_conflict) == false)
            return false;
    }

    return true;
}

