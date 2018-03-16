#ifndef MAINCPPPREPARECMD_H
#define MAINCPPPREPARECMD_H

#include "fileprocessor_a.h"

class MainCppPrepareCmd : public FileCmd
{
public:
    MainCppPrepareCmd(const QString &filenam, const QString& mainwclass);

    // FileCmd interface
public:
    virtual QString process(const QString &input);
    virtual QString name();

private:
    QString m_mainwinclass;
};

#endif // MAINCPPPREPARECMD_H
