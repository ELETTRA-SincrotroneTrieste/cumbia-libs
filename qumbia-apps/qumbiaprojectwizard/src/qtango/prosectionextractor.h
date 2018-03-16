#ifndef PROSECTIONEXTRACTOR_H
#define PROSECTIONEXTRACTOR_H

#include <QString>
#include <QMap>

class ProSectionExtractor
{
public:
    ProSectionExtractor(const QString& path);

    QString get(const QString& section);

    bool error() const { return m_err; }

    QString errorMessage() const { return m_msg; }

private:
    QMap<QString, QString> m_sections;

    bool m_err;

    QString m_msg;
};

#endif // PROSECTIONEXTRACTOR_H
