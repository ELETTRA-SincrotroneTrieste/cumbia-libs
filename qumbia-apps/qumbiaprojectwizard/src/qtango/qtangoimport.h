#ifndef QTANGOIMPORT_H
#define QTANGOIMPORT_H

#include <QString>
#include <QMap>
#include <QObject>
#include <QFileInfoList>
#include "findreplace.h"
#include "pro_files_helper.h"
#include "fileprocessor_a.h"

class QDir;
class QFile;

class QTangoImport : public QObject
{
    Q_OBJECT
public:
    QTangoImport();

    bool open(const QString& pro_filepath);

    QMap<QString, QString> getAppProps() const;

    QMap<QString, QString> getProjectFiles() const;

    QString projectName() const;

    QString projectDir() const;

    QString projectFilePath() const;

    QString mainWidgetVarName() const;

    QString mainWidgetName() const;

    QString toCumbiaPro(const QString& qtango_pro) const;

    QString errorMessage() const;

    bool error() const;

    bool convert();

    bool findFilesRelPath();

    bool outputFile(const QString& name, const QString& path);

    QString getContents(const QString& filenam) const;

    QStringList convertedFileList() const;

signals:
    void newLog(const QList<OpQuality> &log);

    void conversionFinished(bool ok);

    void outputFileWritten(const QString& name, const QString& relpath, bool ok);

private:
    QString m_get_file_contents(const QString& filepath);
    bool m_checkPro(QFile& f);
    bool m_checkCpp(const QString& f);
    bool m_checkH(const QString& h);
    bool m_getMainWidgetProps(const QDir &wdir, QMap<QString, QString> &props);
    bool m_findMainProjectFiles(const QDir &wdir);

    QFileInfoList m_findFiles(QDir wdir, const QString& filter) const;
    QString m_findFile(QDir wdir, const QString& name) const;

    QString m_errMsg;
    QString m_projdir, m_pro_file_path;
    bool m_err;

    ProjectFilesHelper m_proFHelper;

    QMap<QString, QString> m_contents;
    QMap<QString, QString> m_outFileRelPaths;

   // QList<OpQuality> m_log;
};

#endif // QTANGOIMPORT_H
