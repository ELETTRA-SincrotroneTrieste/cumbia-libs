#ifndef QUWIDGETCONTEXTMENUSTRATEGYI_H
#define QUWIDGETCONTEXTMENUSTRATEGYI_H

class QMenu;
class QWidget;
class CuContextI;

/**
 * @brief The QuWidgetContextMenuStrategyI class provides a menu that is shown within
 *        a context menu event.
 *
 * Subclasses must implement createMenu. In this method they will typically add QActions to
 * the menu and connect them to some widget slots.
 */
class QuWidgetContextMenuStrategyI
{
public:
    virtual ~QuWidgetContextMenuStrategyI() {}

    /**
     * @brief createMenu returns a QMenu with the actions available for the widget w
     * @param w the widget where the context menu event takes place.
     * @return a new QMenu with the actions available for the widget w
     *
     * \note
     * This method will normally create QActions and connect them to the widget's slots.
     *
     */
    virtual QMenu* createMenu(QWidget *w) = 0;
};

#endif // QUWIDGETCONTEXTMENUSTRATEGYI_H

