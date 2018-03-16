#ifndef PROCONVERTCMD_H
#define PROCONVERTCMD_H

#include "fileprocessor_a.h"

class ProConvertCmd : public FileCmd
{
public:
    ProConvertCmd(const QString &filenam);

    // FileCmd interface
public:
    virtual QString process(const QString &input);
    virtual QString name();
private:
    QString m_get_file_contents(const QString &fnam);
    QString m_comment_lines(const QString& s);
    QString m_remove_comments(const QString& s);
};

#endif // PROCONVERTCMD_H
