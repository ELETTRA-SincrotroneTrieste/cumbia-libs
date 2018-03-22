#ifndef MAINWIDGETCPPCONSTRUCTPROCESSCMD_H
#define MAINWIDGETCPPCONSTRUCTPROCESSCMD_H

#include "fileprocessor_a.h"

// ui.setupUi becomes ui->setupUi
// add CumbiaTango parameter to the main widget constructor
// the class destructor destroys the ui
class MainWidgetCppConstructProcessCmd : public FileCmd
{
public:
    MainWidgetCppConstructProcessCmd(const QString& fnam, const QString& mainwclass);

    // FileCmd interface
public:
    virtual QString process(const QString &input);
    virtual QString name();

private:
    QString m_mainclass;
};

#endif // MAINWIDGETCPPCONSTRUCTPROCESSCMD_H
