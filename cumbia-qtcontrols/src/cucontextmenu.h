#ifndef INFOCONTEXTMENUFILTER_H
#define INFOCONTEXTMENUFILTER_H

#include <QMenu>
class CuContextI;
class QuActionExtensionPluginInterface;
class CuData;

/** \brief A QObject filter that installs a minimal context menu with an action
 *         to trigger a link statistics request.
 *
 */
class CuContextMenu : public QMenu
{
    Q_OBJECT
public:
    explicit CuContextMenu(QWidget *parent, CuContextI *parent_as_cwi);

private slots:
    void onInfoActionTriggered();
    void onHelperAActionTriggered();
    void onDataReady(const CuData& da);

signals:
    /**
     * \brief This signal is emitted when the used activates the <em>Link statistics</em> action
     *        provided by the menu.
     *
     * @param sender the object that sent the request, as a QWidget
     * @param sender_cwi the object that sent the request, as a CuContextWidgetI (provides access
     *        to the widget context
     *
     * @see CuContextMenu
     *
     */
    void linkStatsTriggered(QWidget *sender, CuContextI *sender_cwi);

public slots:

private:
    CuContextI *m_parent_as_cwi;
    QuActionExtensionPluginInterface *m_action_extensions;
};

#endif // INFOCONTEXTMENUFILTER_H
