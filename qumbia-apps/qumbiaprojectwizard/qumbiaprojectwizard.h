#ifndef QTANGOPROJECTWIZARD_H
#define QTANGOPROJECTWIZARD_H

#include <QWidget>

/** \mainpage
 *
 *  *qumbiaprojectwizard* is an application to generate an empty *cumbia* project. It can also be used
 *         to migrate a simple *QTango* project to *cumbia*.
 *
 * \par Create a new *cumbia* project
 *
 * To start creating a *cumbia project* with *qumbiaprojectwizard* read \ref tutorial_qumbiatango
 *
 * Command
 * \code
 * cumbia new project
 * \endcode
 *
 * \par Migrate a project from QTango
 * To migrate a *QTango project* to *cumbia* read \ref tutorial_from_qtango
 *
 * Command
 * \code
 * cumbia import
 * \endcode
 *
 * *
 *  \section related_readings Related readings
 *
 * \subsection tutorials Tutorials
 *
 * |Tutorials                                     | Module               |
 * |-------------------------------------------------------------------|:--------------------------:|
 * |  <a href="../../cumbia/html/tutorial_cuactivity.html">Writing a *cumbia* activity</a> | <a href="../../cumbia/html/index.html">cumbia</a> |
 * |  <a href="../../cumbia-tango/html/tutorial_activity.html">Writing an activity</a> | <a href="../../cumbia-tango/html/index.html">cumbia-tango</a> |
 * |  <a href="../../cumbia-tango/html/cudata_for_tango.html">CuData for Tango</a> | <a href="../../cumbia-tango/html/index.html">cumbia-tango</a> |
 * |  <a href="../../qumbia-tango-controls/html/tutorial_cumbiatango_widget.html">Writing a Qt widget that integrates with cumbia</a> | <a href="../../qumbia-tango-controls/html/index.html">qumbia-tango-controls</a>  |
 * |  <a href="../../cuuimake/html/cuuimake.html">Using <em>cumbia ui make</em></a> to process Qt designer UI files | <a href="../../cuuimake/html/index.html">qumbia-apps/cuuimake</a>  |
 * |  <a href="../../qumbiaprojectwizard/html/tutorial_qumbiatango.html">Writing a <em>Qt application</em> with cumbia and Tango</em></a>. |<a href="../../qumbiaprojectwizard/html/index.html">qumbia-apps/qumbiaprojectwizard</a>  |
 * |  <a href="../../qumbiaprojectwizard/html/tutorial_from_qtango.html">Porting a <em>QTango application</em> to <em>cumbia-tango</em></a>. |<a href="../../qumbiaprojectwizard/html/index.html">qumbia-apps/qumbiaprojectwizard</a>  |
 * |  <a href="../../qumbianewcontrolwizard/html/tutorial_qumbianewcontrolwizard.html"><em>cumbia new control</em></a>: quickly add a custom Qt widget to a cumbia project | <a href="../../qumbianewcontrolwizard/html/index.html">qumbia-apps/qumbianewcontrolwizard</a>  |
 * |  <a href="../../cumbia-qtcontrols/html/understanding_cumbia_qtcontrols_constructors.html">Understanding <em>cumbia-qtcontrols constructors, sources and targets</em></a> |<a href="../../cumbia-qtcontrols/html/index.html">cumbia-qtcontrols</a>. |
 *
 * <br/>
 * \subsection cumodules Modules
 *
 * |Other *cumbia* modules  |
 * |-------------------------------------------------------------------|
 * | <a href="../../cumbia/html/index.html">cumbia module</a>. |
 * | <a href="../../cumbia-tango/html/index.html">cumbia-tango module</a>. |
 * | <a href="../../cumbia-qtcontrols/html/index.html">cumbia-qtcontrols module</a>.  |
 * | <a href="../../qumbia-tango-controls/html/index.html">cumbia-qtcontrols module</a>.  |
 * | <a href="../../cumbia-epics/html/index.html">qumbia-epics module</a>.   |
 * | <a href="../../qumbia-epics-controls/html/index.html">qumbia-epics-controls module</a>.  |
 *
 *
 * <br/>
 * \subsection cu_apps apps
 *
 * Shortcuts for the common operations provided by the *qumbia-apps* applications are defined as follows:
 *
 *
 * |Applications (command line)   | description                                 | app
 * |------------------------------|--------------------------------------------|:---------------:|
 * | *cumbia new project*          | create a new cumbia project               |<a href="../../qumbiaprojectwizard/html/index.html">qumbia-apps/qumbiaprojectwizard</a>  |
 * | *cumbia import*               | migrate a QTango project into cumbia      |<a href="../../qumbiaprojectwizard/html/index.html">qumbia-apps/qumbiaprojectwizard</a>  |
 * | *cumbia new control*          | write a *cumbia control* reader or writer | <a href="../../qumbianewcontrolwizard/html/index.html">qumbia-apps/qumbianewcontrolwizard</a>  |
 * | *cumbia ui make*              | run *cuuimake* to generate *qt+cumbia* ui_*.h files | <a href="../../cuuimake/html/index.html">qumbia-apps/cuuimake</a>  |
 * | *cumbia client*               | run a generic cumbia client | <a href="../../cumbia_client/html/index.html">qumbia-apps/cumbia_client</a>  |
 *
 */

class QTangoImport;

namespace Ui {
class QumbiaProjectWizard;
}

class QRadioButton;

class MyFileInfo
{
public:
    MyFileInfo() {}

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

    void onConversionDialogFinished();

    void qmlToggled(bool t);

    void addAndroidLib();

    void removeAndroidLib();

    void initAndroidLibs();

private:
    void checkValidity();

    void setValid(QWidget *w, bool valid);

    void loadSettings();

    void addProperties(QString &uixml);


    QStringList findSupportedFactories();

    void m_setAppProps(const QMap<QString, QString>& props);

    void m_setProjectFiles(const QMap<QString, QString>& props);

    void m_saveUISettings();

    void m_launchApps(const QString& path);

    QTangoImport *m_qtangoImport;

private:
    Ui::QumbiaProjectWizard *ui;

    QString m_selectedFactory;

    QRadioButton* m_rbFactorySaveChecked;

    QString m_formatAndroidLibs() const;
};

#endif // QTANGOPROJECTWIZARD_H
