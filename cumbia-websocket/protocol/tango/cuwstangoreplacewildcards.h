#ifndef CUWSTANGOREPLACEWILDCARDS_H
#define CUWSTANGOREPLACEWILDCARDS_H

#include <qureplacewildcards_i.h>
#include <QStringList>

class CuWsTangoReplaceWildcards : public QuReplaceWildcards_I
{
public:
    ~CuWsTangoReplaceWildcards() {}

    // QuReplaceWildcards_I interface
public:
    QString replaceWildcards(const QString &s, const QStringList &args) const;
};

#endif // CUWSTANGOREPLACEWILDCARDS_H
