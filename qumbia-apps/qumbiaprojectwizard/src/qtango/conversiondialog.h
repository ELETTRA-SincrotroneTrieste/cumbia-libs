#ifndef CONVERSIONDIALOG_H
#define CONVERSIONDIALOG_H

#include <QDialog>
#include "fileprocessor_a.h"
#include <QList>

class ConversionDialog : public QDialog
{
    Q_OBJECT
public:
    ConversionDialog(QWidget* parent, const QString &outpath);

    ~ConversionDialog();

    QString outputPath() const;

public slots:
    void addLogs(const QList<OpQuality>& log);

    void conversionFinished(bool ok);

    void outputFileWritten(const QString& name, const QString& filerelpath, bool ok);

    void changeOutPath();

    void slotOkClicked();

private slots:
    bool m_setProblemWidgetsVisible(bool v);

signals:
    void okClicked();
};

#endif // CONVERSIONDIALOG_H
