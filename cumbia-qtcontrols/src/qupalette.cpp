#include "qupalette.h"

/* this was from elettracolors.h
 */
/*
#define EGREEN 		QColor(93, 234, 78)
#define EYELLOW  	QColor(247, 248, 156)
#define EGRAY  		QColor(209,207,204)
#define ECYAN 		QColor(185,222,219)
#define ERED   		QColor(250, 80, 80)
#define EBLUE  		QColor(100, 131, 201)
#define EORANGE		QColor(243, 186, 109)
#define EVIOLET 	QColor(200, 148, 208)
#define EDARK_GREEN	QColor(92, 151, 109)
#define EMAROON		QColor(157, 137, 137)
#define EPINK		QColor(231, 182, 226)
#define EFUCHSIA	QColor(240, 130, 228)
#define EBLACK		Qt::black
#define ELIGHT_GRAY	QColor(231, 182, 226)
#define EDARK_GRAY	QColor(155,155,155)
*/

/** \brief class constructor.
 *
 */
QuPalette::QuPalette()
{
    insert(QString("blue"), QColor(90, 121, 198));
    insert(QString("gray"), QColor(199,197,194));
    insert(QString("red"), QColor(255, 75, 76));
    insert(QString("dark_red"), QColor(235, 55, 56));
    insert(QString("green"), QColor(90, 240, 77));
    insert(QString("dark_green"), QColor(50, 200, 47));
    insert(QString("light_green"), QColor(195,255,182));
    insert(QString("dark_gray"), QColor(145,145,145));
    insert(QString("light_gray"), QColor(231, 182, 226));
    insert(QString("yellow"), QColor(244, 245, 153));
    insert(QString("dark_yellow"), QColor(Qt::darkYellow));
    insert(QString("orange"), QColor(238, 181, 104));
    insert(QString("violet"), QColor(200, 148, 208));
    insert(QString("pink"), QColor(231, 182, 226));
    insert(QString("black"), QColor(Qt::black));
    insert(QString("white"), QColor(Qt::white));
    insert(QString("white1"), QColor(253, 253, 253));
    insert(QString("white2"), QColor(252, 252, 252));
    insert(QString("white3"), QColor(251, 251, 251));
    insert(QString("white4"), QColor(250, 250, 250));
    insert(QString("white5"), QColor(249, 249, 249));
}
