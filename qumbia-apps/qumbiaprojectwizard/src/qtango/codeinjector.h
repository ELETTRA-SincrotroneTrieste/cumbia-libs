#ifndef CODEINJECTORS_H
#define CODEINJECTORS_H

#include <QString>
#include "cumbiacodeinjectcmd.h"

class CodeInjector
{
public:
    CodeInjector(const QString& filename, const QString& mainwidclass, const QString& mainwidvar);

    /** \brief injects the code from the sections into the file specified in the class constructor
     *
     * @return a string with the modified code
     *
     */
    virtual QString inject(const QString& input, const QList<Section>& sections);

    bool error() const {
        return m_err;
    }

    QString errorMessage() const {
        return m_errMsg;
    }

    bool write_out(const QString& outf, const QString &s);

    QList<OpQuality> getLog() const {
        return m_log;
    }

protected:
    bool m_err;
    QString m_filename, m_errMsg;
    QString m_mainwclass, m_mainwvar;
    QList<OpQuality> m_log;
};

#endif // CODEINJECTORS_H
