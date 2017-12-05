#ifndef INFOCONTEXTMENUFILTER_H
#define INFOCONTEXTMENUFILTER_H

#include <QMenu>

/** \brief A QObject filter that installs a minimal context menu with an action
 *         to trigger a link statistics request.
 *
 */
class CuContextMenu : public QMenu
{
    Q_OBJECT
public:
    explicit CuContextMenu(QWidget *parent);

protected:
private slots:
    void onInfoActionTriggered();

signals:
    void linkStatsTriggered(QWidget *sender);

public slots:
};

#endif // INFOCONTEXTMENUFILTER_H
