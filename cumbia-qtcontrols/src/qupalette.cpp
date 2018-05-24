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

QuPalette::QuPalette()
{
    insert("blue", QColor(90, 121, 198));
    insert("gray", QColor(199,197,194));
    insert("red", QColor(255, 75, 76));
    insert("dark_red", QColor(235, 55, 56));
    insert("green", QColor(90, 240, 77));
    insert("dark_green", QColor(50, 200, 47));
    insert("dark_gray", QColor(145,145,145));
    insert("light_gray", QColor(231, 182, 226));
    insert("yellow", QColor(244, 245, 153));
    insert("orange", QColor(238, 181, 104));
    insert("violet", QColor(200, 148, 208));
    insert("pink", QColor(231, 182, 226));
    insert("black", QColor(Qt::black));
    insert("white", QColor(Qt::white));
}
