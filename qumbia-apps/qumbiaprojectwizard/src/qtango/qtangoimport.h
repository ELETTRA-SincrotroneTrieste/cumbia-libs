#ifndef QTANGOIMPORT_H
#define QTANGOIMPORT_H

#include <QString>
#include <QMap>
#include <QFileInfoList>
#include "findreplace.h"

class QDir;
class QFile;

class QTangoImport
{
public:
    QTangoImport();

    bool open(const QString& pro_filenam);

    QMap<QString, QString> getAppProps() const;

    QMap<QString, QString> getProjectFiles() const;

    QString projectName() const;

    QString projectDir() const;

    QString mainWidgetVarName() const;

    QString mainWidgetName() const;

    QString toCumbiaPro(const QString& qtango_pro) const;

    QString errorMessage() const;

    bool error() const;

    bool convert();

    FindReplace conversionDefs() const;

private:
    QMap<QString, QString> m_appPropMap, m_proFiles;

    bool m_checkPro(QFile& f);
    bool m_checkCpp(const QString& f);
    bool m_checkH(const QString& h);
    bool m_getMainWidgetProps(const QDir &wdir, QMap<QString, QString> &props);
    bool m_findMainProjectFiles(const QDir &wdir);

    QFileInfoList m_findFiles(QDir wdir, const QString& filter) const;
    QString m_findFile(QDir wdir, const QString& name) const;

    QString m_errMsg;
    QString m_projnam, m_mainwidgetvarnam, m_mainwidgetnam;
    QString m_projdir;
    bool m_err;

    FindReplace m_conversionDefs;
};

#endif // QTANGOIMPORT_H
