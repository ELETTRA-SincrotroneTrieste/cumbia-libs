#ifndef QUREPLACEWILDCARDS_I_H
#define QUREPLACEWILDCARDS_I_H

#include <QString>

/*!
 * \brief The QuReplaceWildcards_I interface can be used to implement command line arguments
 *        wildcard replacement
 *
 * Implement the pure virtual method replaceWildcards in subclasses to parse the command line
 * arguments and replace into a string with wildcards the corresponding argument.
 *
 * \par Example
 *
 * ./bin/my_application server1 server2
 *
 * s1 = "$1/read";
 * s2 = "$2/read";
 *
 * replaceWildcards will return
 *
 * s_1 = "server1/read";
 * s_2 = "server2/read";
 */
class QuReplaceWildcards_I
{
public:
    virtual ~QuReplaceWildcards_I() {}

    virtual QString replaceWildcards(const QString &s, const QStringList& args) const = 0;
};

#endif // QUREPLACEWILDCARDS_I_H
