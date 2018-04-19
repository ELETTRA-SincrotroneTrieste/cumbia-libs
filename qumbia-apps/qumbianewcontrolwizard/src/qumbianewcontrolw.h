#ifndef QUMBIANEWCONTROLW_H
#define QUMBIANEWCONTROLW_H

#include <QWidget>
#include <QMap>

/*! \mainpage
 *
 * *qumbianewcontrolwizard* is an utility to generate the skeleton of a *cumbia* reader, writer or reader *and* writer.
 *
 * To get started, please refer to \ref tutorial_qumbianewcontrolwizard
 *
 * \par command line
 * \code
 * cumbia new control
 * \endcode
 *
 *
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

class QLineEdit;

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private:
    Ui::Widget *ui;

private slots:
    void nameTextChanged(const QString& txt);
    void setTextEdited(const QString& s = QString());

    void changeOutDir();

    void generate();

    void outDirTextChanged(const QString &s);

    void parentClassNameChanged(const QString& cn);

    void m_genIncludeMap();

    void updateBrief();

private:
    QMap<QString, QString> m_includeMap;

    void m_add_to_pro();

    QString m_replace_from_template(const QString& templ);

    QString m_getTemplateContents(const QLineEdit *le);

    bool m_error;

    QString m_message;
};

#endif // QUMBIANEWCONTROLW_H
