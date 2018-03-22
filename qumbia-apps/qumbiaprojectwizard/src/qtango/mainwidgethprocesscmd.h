#ifndef MAINWIDGETHPROCESSCMD_H
#define MAINWIDGETHPROCESSCMD_H

#include "fileprocessor_a.h"

/*! \brief If mainwidget.h class declaration contains a Ui::MainwidgetName ui,
 *         check that it is a pointer type. If not, convert
 *
 */
class MainWidgetHProcessCmd : public FileCmd
{
public:
    MainWidgetHProcessCmd(const QString &fnam, const QString &mainwnam);

    // FileCmd interface
public:
    virtual QString process(const QString &input);
    virtual QString name();

private:
    QString m_mainwnam; // main widget class name
};

#endif // MAINWIDGETHPROCESSCMD_H
