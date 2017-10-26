#ifndef PROCESSOR_H
#define PROCESSOR_H

class Substitutions;

#include <QStringList>
#include <QMap>
#include "defs.h"

class Processor
{
public:
    Processor();

    bool expand(const Substitutions& subs, const QMap<QString,
                Expand>& widgetExpMap, const QString& ui_h_fname,
                const SearchDirInfoSet &dirInfoSet);

    QMap<QString, bool> findUI_H(const SearchDirInfoSet &dirInfoSet);

    void setDebug(bool d) { m_debug = d; }

    bool error() const { return m_error; }

    QString lastError() const { return m_lastError;}

private:
    bool m_debug;

    bool m_error;

    QString m_getUiHFileName(const QString& ui_h_fname, const SearchDirInfoSet &dirInfoSet);

    QString m_lastError;
};

#endif // PROCESSOR_H
