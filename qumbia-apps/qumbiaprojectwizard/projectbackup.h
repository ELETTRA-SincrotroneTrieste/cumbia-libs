#ifndef PROJECTBACKUP_H
#define PROJECTBACKUP_H

#include <QString>

class ProjectBackup
{
public:
    enum Result { BackedUp, BackupUnnecessary, BackupFailed, NeedsBackup, Cancel };

    ProjectBackup(const QString& dir);

    Result popup_message() const;

    Result save(const QString& backupdir) const;

private:
    QString m_dirnam;
    bool m_copyRecursively(QString from_dir, QString to_dir, bool replace_on_conflict = true) const;
};

#endif // PROJECTBACKUP_H
