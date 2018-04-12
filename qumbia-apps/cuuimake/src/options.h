#ifndef OPTIONS_H
#define OPTIONS_H

#include <QStringList>
#include <QVariant>
#include <QMap>

class CuUiMake;

class Options
{
public:
    Options(const QStringList& args);

    QVariant getopt(const QString& name);

    QStringList help(const QString& param);

    bool error() const { return m_error; }

    QString lastError() const { return m_lastError; }

    void printHelp(const CuUiMake &cm) const;

    void printOptionsList(const CuUiMake& cm) const;

    void printOptions(const CuUiMake& cm, const QStringList &theseOptions = QStringList()) const;

    QString make_j() const;

    bool configure(const CuUiMake & cm);

private:
    QMap<QString, QVariant> m_map;
    QMap<QString, QString> m_helpMap;

    bool m_error;
    QString m_lastError;
};

#endif // OPTIONS_H
