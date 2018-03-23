#ifndef PROJECTFILESHELPER_H
#define PROJECTFILESHELPER_H

class QDir;

#include <QString>
#include <QMap>
#include <QFileInfoList>
#include <QDir>

class ProjectFilesHelper
{
public:
    ProjectFilesHelper();

    ProjectFilesHelper(const QString &projnam);

    bool findMainWidgetProps(const QDir &wdir);
    bool findMainProjectFiles(const QDir &wdir);
    QFileInfoList findFiles(QDir wdir, const QString &filter, const QString &exclude_regexp = QString()) const;
    QString findFile(QDir wdir, const QString &name) const;

    QMap<QString, QString> projectFilesMap() const {
        return m_proFiles;
    }

    QMap<QString, QString> appPropMap() const {
        return m_appPropMap;
    }

    bool error() const {
        return m_err;
    }

    QString errorMessage() const {
        return m_errMsg;
    }

    /** \brief returns the ClassName in namespace ui
     *
     * \par example
     * The PS2605 in the Ui namespace:
     * Ui::PS2605
     */
    QString uiClassName() const {
        return m_uiclassnam;
    }

    QString mainWidgetClassName() const {
        return m_mainwclassnam;
    }

    QString mainWidgetVarName() const {
        return m_mainwidgetvarnam;
    }

    QString projectName() const {
        return m_projnam;
    }

private:
    QMap<QString, QString> m_proFiles, m_appPropMap;
    QString m_errMsg;
    bool m_err;

    QString m_projnam, m_mainwidgetvarnam, m_uiclassnam;
    QString m_mainwclassnam;
    QString m_projdir;

    QString m_findWidgetClassName(const QString& filenam);
};

#endif // PROJECTFILESHELPER_H
