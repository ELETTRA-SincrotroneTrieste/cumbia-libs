#ifndef CONVERSIONDEFS_H
#define CONVERSIONDEFS_H

#include <QString>
#include <QList>

#include "fileprocessor_a.h"

class FindReplace : public FileCmd
{
public:
    FindReplace(const QString& filename);

    ~FindReplace();

    // FileProcCommand interface
    QString process(const QString &input);

    // FileCmd interface
public:
    QString name();

private:
    QString m_comment_line(const QString& line, bool xml);

    QString m_comment_add(const QString& after, bool xml);
};

#endif // CONVERSIONDEFS_H
