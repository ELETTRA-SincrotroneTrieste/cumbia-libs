#ifndef CUUIMAKE_H
#define CUUIMAKE_H

#include <QString>

class Options;

/*! \mainpage
 *
 * cuuimake is an utility to generate ui_*.h files from Qt designer *ui* xml forms.
 * The cumbia widgets' arguments in class constructors in the ui_*.h files produced by cuuimake
 * are expanded to be compatible with cumbia-qtcontrols class constructors.
 *
 * Please refer to the <a href="cuuimake.html">cumbia ui make, tool to configure a Qt project with cumbia widgets</a>
 * page to get started immediately.
 *
 * *cuuimake* can also be invoked with the shortcut
 *
 * \code
 * cumbia ui make
 * \endcode
 *
 * if the *qumbia-apps* have been installed thoroughly (i.e. qmake, make and *sudo make install*
 * have been executed from the *qumbia-apps* top level directory)
 */
class CuUiMake
{
public:

    enum Step { Uic, Analysis, Expand, Conf, Help, Info, QMake, Make, Clean, Doc };

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
