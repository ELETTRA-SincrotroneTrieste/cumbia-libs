#ifndef CUUIMAKE_H
#define CUUIMAKE_H

#include <QString>

class CuUiMake
{
public:

    enum Step { Uic, Analysis, Expand };

    CuUiMake();

    void print(Step step, bool err, const char *fmt, ...);

    const char *toc(const QString& s) const;

    bool make();

private:
    bool m_debug;

    QString m_findLocalConfFile()  const;
};

#endif // CUUIMAKE_H
