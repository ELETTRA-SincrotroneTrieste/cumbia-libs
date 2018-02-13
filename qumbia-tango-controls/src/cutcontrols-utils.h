#ifndef CUTCONTROLSUTILS_H
#define CUTCONTROLSUTILS_H

#include <QStringList>

/*! \brief utility class offering some functionalities useful to qumbia-tango-controls
 *         components.
 *
 * Currently, the following functions are offered:
 *
 * \li replace wildcards from sources and targets
 *
 */
class CuTControlsUtils
{
public:
    CuTControlsUtils();

    QString replaceWildcards(const QString &s, const QStringList& args);
};

#endif // CUTCONTROLSUTILS_H
