#ifndef CUHTTPTANGOREPLACEWILDCARDS_H
#define CUHTTPTANGOREPLACEWILDCARDS_H

#include <qureplacewildcards_i.h>
#include <QStringList>

class CuHttpTangoReplaceWildcardsPrivate;

class CuHttpTangoReplaceWildcards : public QuReplaceWildcards_I
{
public:
    CuHttpTangoReplaceWildcards(const QStringList& args = QStringList());

    ~CuHttpTangoReplaceWildcards();

    // QuReplaceWildcards_I interface
public:
    QString replaceWildcards(const QString &s, const QStringList &args) const;

private:
    CuHttpTangoReplaceWildcardsPrivate* d;
};

#endif // CUHTTPTANGOREPLACEWILDCARDS_H
