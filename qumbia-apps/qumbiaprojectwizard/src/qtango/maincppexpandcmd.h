#ifndef MAINCPPEXPANDCMD_H
#define MAINCPPEXPANDCMD_H

#include "fileprocessor_a.h"

class MainCppExpandCmd : public FileCmd
{
public:
    MainCppExpandCmd(const QString& fnam, const QString& mainwnam);

    // FileCmd interface
public:
    virtual QString process(const QString &input);
    virtual QString name();

private:
    QString m_mainwclass;
};

#endif // MAINCPPEXPANDCMD_H
