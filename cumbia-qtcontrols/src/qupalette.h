#ifndef QUPALETTE_H
#define QUPALETTE_H

#include <QMap>
#include <QString>
#include <QColor>

class QuPalette : public QMap<QString, QColor>
{
public:
    QuPalette();
};

#endif // QUPALETTE_H
