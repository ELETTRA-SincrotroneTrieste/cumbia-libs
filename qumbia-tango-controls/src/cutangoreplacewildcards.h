#ifndef CUTANGOREPLACEWILDCARDS_H
#define CUTANGOREPLACEWILDCARDS_H

#include <qureplacewildcards_i.h>

class CuTangoReplaceWildcards : public QuReplaceWildcards_I {
    // QuReplaceWildcards_I interface
public:
    QString replaceWildcards(const QString &s, const QStringList &args) const;
};


#endif // CUTANGOREPLACEWILDCARDS_H
