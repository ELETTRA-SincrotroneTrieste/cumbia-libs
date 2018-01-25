#ifndef UICPROC_H
#define UICPROC_H

#include <QString>

class SearchDirInfoSet;

class UicProc
{
public:
    UicProc();

    QString getUicCmd(const QString& ui_h_fname,
                      const SearchDirInfoSet &dirInfoSet);

    bool run(const QString& cmd);

    QString lastError() const { return m_lastError; }

    bool error() const { return m_error; }

private:
    bool m_error;
    QString m_lastError;
};

#endif // UICPROC_H
