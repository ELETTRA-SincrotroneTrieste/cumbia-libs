#ifndef CMDLINEOPTIONS_H
#define CMDLINEOPTIONS_H

#include <QStringList>
#include <QMap>
#include "rconfig.h"

class CmdLineOptions
{
public:
    CmdLineOptions(bool formula_plugin_enabled, bool historical_db_plugin_enabled);

    RConfig parse(const QStringList &args) const;

    void help(const QString &appname, const QString& modulenam) const;

    void list_options() const;

private:
    QMap<QString, QString> m_help_map;
};

#endif // CMDOPTIONS_H
