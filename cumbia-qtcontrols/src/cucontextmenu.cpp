#include "cucontextmenu.h"
#include <QAction>

/** \brief Creates a QMenu with a minimal set of actions.
 *
 * The following actions are created by the CuContextMenuConstructor:
 *
 * \li A Link statistics action, to request information about the underlying link.
 *
 * @param parent: the parent widget of this menu.
 * @param parent_as_cwi: the same object as parent must be passed. It must implement CuContextI
 *        interface.
 *
 * \par Note.
 * Since it is not possible to cast CuContextI to QWidget and vice versa, it is necessary
 * to store the parent reference both as QWidget and CuContextI.
 * The linkStatsTriggered signal will contain the CuContextI pointer that will be used
 * by the receiver to get the CuContext reference to access the cumbia-qtcontrols widget
 * context.
 */
CuContextMenu::CuContextMenu(QWidget *parent, CuContextI *parent_as_cwi) : QMenu(parent)
{
    QAction *info = new QAction("Link stats...", this);
    connect(info, SIGNAL(triggered(bool)), this, SLOT(onInfoActionTriggered()));
    addAction(info);
    m_parent_as_cwi = parent_as_cwi;
}

void CuContextMenu::onInfoActionTriggered()
{
    emit linkStatsTriggered(parentWidget(), m_parent_as_cwi);
}
