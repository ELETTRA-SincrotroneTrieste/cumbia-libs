#include "quplotcontextmenucomponent.h"
#include "quwidgetcontextmenustrategyi.h"
#include <QPrinter>
#include <QPrintDialog>
#include <QMenu>
#include <QMessageBox>

#include <quplot_base.h>
#include <qwt_plot_renderer.h>

#include "snapshot.h"
#include "plotsaver.h"

QuPlotContextMenuComponent::QuPlotContextMenuComponent()
{

}

QString QuPlotContextMenuComponent::name() const
{
    return "context_menu";
}

void QuPlotContextMenuComponent::attachToPlot(QuPlotBase *)
{

}

void QuPlotContextMenuComponent::connectToPlot(QuPlotBase *)
{

}

void QuPlotContextMenuComponent::execute(QuPlotBase *plot,
                                         QuWidgetContextMenuStrategyI*
                                         ctxMenuStrategy,
                                         const QPoint &pos)
{
    Q_UNUSED(pos);
    if(ctxMenuStrategy)
    {
        QMenu *menu = ctxMenuStrategy->createMenu(plot);
        menu->exec(QCursor::pos());
        delete menu;
    }
}

void QuPlotContextMenuComponent::print(QuPlotBase *p)
{
    QPrinter printer;
    QString docName = p->title().text();
    if (docName.isEmpty() )
    {
        docName.replace (QRegExp ("\n"), "--");
        printer.setDocName (docName);
    }
    printer.setCreator("QuPlot");
    QPrintDialog dialog(&printer);
    if ( dialog.exec() )
    {
        QwtPlotRenderer renderer;
        renderer.renderTo(p, printer);
    }
}

void QuPlotContextMenuComponent::snapshot(QuPlotBase *p)
{
    /* temporary enable the title on the canvas, saving its visibility
   * state for subsequent restore.
   */
    bool saveTitleShown = p->titleOnCanvasEnabled();
    p->setTitleOnCanvasEnabled(true);
    PlotSnapshot pSnap;
    pSnap.grab(p);
    p->setTitleOnCanvasEnabled(saveTitleShown);
}

void QuPlotContextMenuComponent::copyImage(QuPlotBase *p)
{
    /* temporary enable the title on the canvas, saving its visibility
   * state for subsequent restore.
   */
    bool saveTitleShown = p->titleOnCanvasEnabled();
    p->setTitleOnCanvasEnabled(true);
    //   qApp->processEvents(); /* force the QApplication refresh of the widget before grabbing it */
    PlotSnapshot pSnap;
    pSnap.copy(p);
    p->setTitleOnCanvasEnabled(saveTitleShown);
}

void QuPlotContextMenuComponent::saveData(QuPlotBase *p)
{
    PlotSaver plotSaver;
    if(!plotSaver.save(p->curves()))
        QMessageBox::critical(p, "Error saving on file", QString("Error saving file \"%1\":\n%2").arg(plotSaver.fileName()).
                              arg(plotSaver.errorMessage()));
}

void QuPlotContextMenuComponent::configurePlot(QuPlotBase *p)
{
    QDialog *cdialog = p->createConfigureDialog();
    cdialog->show();
}

