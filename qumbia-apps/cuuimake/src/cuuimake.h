#ifndef CUUIMAKE_H
#define CUUIMAKE_H

#include <QString>

class Options;

class CuUiMake
{
public:

    enum Step { Uic, Analysis, Expand, Conf, Help, Info, QMake, Make, Clean };

    CuUiMake();

    ~CuUiMake();

    void print(Step step, bool err, const char *fmt, ...) const;

    const char *toc(const QString& s) const;

    bool make();

    bool dummy() const { return m_dummy; }

private:
    bool m_debug;

    bool m_dummy;

    Options *m_options;

    QString m_findLocalConfFile()  const;
};

#endif // CUUIMAKE_H
