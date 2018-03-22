#ifndef CONVERSIONDEFS_H
#define CONVERSIONDEFS_H

#include <QString>
#include <QList>
#include <QDialog>

#include "fileprocessor_a.h"

class GetOptionDialog : public QDialog
{
public:
    GetOptionDialog(const QString &comment, const QString &line, const QStringList &options, QWidget *parent);

    int exec();

    QString m_selectedOption;

protected:
    void reject();
};

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

    QString get_option(const QString& text, const QString &snippet, const QStringList& options);
};

#endif // CONVERSIONDEFS_H
