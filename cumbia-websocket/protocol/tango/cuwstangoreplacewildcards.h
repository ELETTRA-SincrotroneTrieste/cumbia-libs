#ifndef CUWSTANGOREPLACEWILDCARDS_H
#define CUWSTANGOREPLACEWILDCARDS_H

#include <qureplacewildcards_i.h>
#include <QStringList>

class CuWsTangoReplaceWildcardsPrivate;

class CuWsTangoReplaceWildcards : public QuReplaceWildcards_I
{
public:
    CuWsTangoReplaceWildcards(const QStringList& args = QStringList());

    ~CuWsTangoReplaceWildcards();

    // QuReplaceWildcards_I interface
public:
    QString replaceWildcards(const QString &s, const QStringList &args) const;

private:
    CuWsTangoReplaceWildcardsPrivate* d;
};

#endif // CUWSTANGOREPLACEWILDCARDS_H
