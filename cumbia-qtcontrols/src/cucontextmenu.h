#ifndef INFOCONTEXTMENUFILTER_H
#define INFOCONTEXTMENUFILTER_H

#include <QMenu>
class CuContext;
class CuContextMenuActionsPlugin_I;
class CuData;

/** \brief A QObject filter that installs a minimal context menu with an action
 *         to trigger a link statistics request.
 *
 */
class CuContextMenu : public QMenu
{
    Q_OBJECT
public:
    explicit CuContextMenu(QWidget *parent, const CuContext *ctx);

private slots:
    void popup_noplugin_msg();

private:
    CuContextMenuActionsPlugin_I *m_ctx_menu_actions;
    const CuContext *m_ctx;
};

#endif // INFOCONTEXTMENUFILTER_H
