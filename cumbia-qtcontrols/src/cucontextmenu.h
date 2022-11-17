#ifndef INFOCONTEXTMENUFILTER_H
#define INFOCONTEXTMENUFILTER_H

#include <QMenu>

class CuContextI;
class CuContextMenuPrivate;
class CuData;

/** \brief A QObject filter that installs a context menu with actions defined by
 *         *external plugins*
 *
 * A default implementation is provided by the
 * qumbia-plugins/widgets-std-context-menu-actions plugin
 *
 * See the CuContextMenu constructor documentation for further details.
 *
 */
class CuContextMenu : public QMenu
{
    Q_OBJECT
public:
    explicit CuContextMenu(QWidget *parent);

    ~CuContextMenu();

    void popup(const QPoint &pos, const CuContextI *ctxi);
    void prepare(const CuContextI *ctxi);
private slots:
    void popup_noplugin_msg();

private:
    CuContextMenuPrivate *d;
};

#endif // INFOCONTEXTMENUFILTER_H
