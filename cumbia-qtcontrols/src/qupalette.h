#ifndef QUPALETTE_H
#define QUPALETTE_H

#include <QMap>
#include <QString>
#include <QColor>

/*! \brief a map associating a color name to a QColor
 *
 * This is the color map:
     * \li  ("blue", QColor(90, 121, 198));
     * \li  ("gray", QColor(199,197,194));
     * \li  ("red", QColor(255, 75, 76));
     * \li  ("dark_red", QColor(235, 55, 56));
     * \li  ("green", QColor(90, 240, 77));
     * \li  ("dark_green", QColor(70, 220, 57));
     * \li  ("light_green", QColor(145,255,132));
     * \li  ("dark_gray", QColor(145,145,145));
     * \li  ("light_gray", QColor(231, 182, 226));
     * \li  ("yellow", QColor(244, 245, 153));
     * \li  ("orange", QColor(238, 181, 104));
     * \li  ("violet", QColor(200, 148, 208));
     * \li  ("pink", QColor(231, 182, 226));
     * \li  ("black", QColor(Qt::black));
     * \li  ("white", QColor(Qt::white));
 */
class QuPalette : public QMap<QString, QColor>
{
public:
    QuPalette();
};

#endif // QUPALETTE_H
