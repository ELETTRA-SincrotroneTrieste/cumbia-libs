#ifndef CUEPALETTEPROCESS_H
#define CUEPALETTEPROCESS_H

#include <QString>
class QWidget;

class CuEPaletteProcess
{
public:
    CuEPaletteProcess();

    bool process(const QString& fname, QString& contents, QWidget *rbcontainer);

    QString message() const;

private:
    QString m_msg;
};

#endif // CUEPALETTEPROCESS_H
