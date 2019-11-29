#ifndef CMDLINEOPTIONS_H
#define CMDLINEOPTIONS_H

#include <QStringList>
#include <QMap>
#include "rconfig.h"

class CmdLineOptions
{
public:
    CmdLineOptions();

    RConfig parse(const QStringList &args) const;

    void usage(const QString& appname) const;

    void help(const QString &appname, const QString& modulenam) const;

    void list_options() const;

private:
    QMap<QString, QString> m_help_map;
};

#endif // CMDOPTIONS_H
