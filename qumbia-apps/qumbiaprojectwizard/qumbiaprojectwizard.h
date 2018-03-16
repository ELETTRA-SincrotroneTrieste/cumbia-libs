#ifndef QTANGOPROJECTWIZARD_H
#define QTANGOPROJECTWIZARD_H

#include <QWidget>

class QTangoImport;

namespace Ui {
class QumbiaProjectWizard;
}

class MyFileInfo
{
public:
    MyFileInfo(const QString &templateFileNam, const QString& newFileNam, const QString& subdirnam);

    QString templateFileName, newFileName, subDirName;
};

class QumbiaProjectWizard : public QWidget
{
    Q_OBJECT

public:
    enum Support { Tango, Epics, TangoEpics };

    explicit QumbiaProjectWizard(QWidget *parent = 0);

    ~QumbiaProjectWizard();

private slots:
    void init();

    void create();

    void checkText(const QString &);

    void selectLocation();

    void addProperty(const QStringList parts = QStringList() << "-" << "-" << "string");

    void removeProperty();

    void projectNameChanged(const QString &);

    void setFactory(bool rbchecked);

    void qtangoImport();

    void importRbToggled(bool);

    void conversionDialogOkClicked();

    void convertStart();

private:
    void checkValidity();

    void setValid(QWidget *w, bool valid);

    void loadSettings();

    void addProperties(QString &uixml);


    QStringList findSupportedFactories();

    void m_setAppProps(const QMap<QString, QString>& props);

    void m_setProjectFiles(const QMap<QString, QString>& props);

    void m_saveUISettings();

    QTangoImport *m_qtangoImport;

private:
    Ui::QumbiaProjectWizard *ui;

    QString m_selectedFactory;
};

#endif // QTANGOPROJECTWIZARD_H
