#ifndef QUPLOT_BASE_H
#define QUPLOT_BASE_H


/***************************************************************************
*   Copyright (C) 2008 by Giacomo Strangolino	   			  *
*   delleceste@gmail.com		   				  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_layout.h>
#include <qwt_scale_widget.h>
#include <qwt_scale_draw.h>
#include <qwt_scale_engine.h>
#include <qwt_date_scale_draw.h>

#include <quwidgetinterface.h>

class QuPlotBasePrivate;
class CuData;
class QuPlotCurve;
class QuWidgetUpdateStrategyI;
class QuWidgetContextMenuStrategyI;
class QuPlotComponent;

/*! @private */
class ShiftClickEater : public QObject
{
    Q_OBJECT
public:
    ShiftClickEater(QObject *parent) : QObject(parent) {}

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

/** \defgroup plots Plots
 * @{
 */


/** \brief A simple plot with zooming and scrolling capabilities
 *
 * \ingroup plots
 *
 * This class provides a simple plot able to zoom and scroll the curves.
 * It should be used with the EPlotCurve as curves to take advantage of
 * the zooming features.
 * You might want to look at the refresh() method to learn how zoom works.
 *
 * QuPlotBase - based plots only manage an Y left and X bottom axes.
 * Should you need other features, you have to use QwtPlot interface in order to add
 * scales other than xBottom and yLeft.
 *
 * <h3>X axis scaling</h3>
 * <p> X axis scaling is managed internally by QuPlotBase. This means that you should
 * use QuPlotBase methods to correctly manage x axis autoscaling.
 * Remember that when you manually set either the <em>upper and/or lower bounds of the x scale</em>,
 * x axis autoscale is disabled. The X axis lower and upper bounds will remain fixed to the values
 * provided. Zoom scrolling is disabled in that scenario.
 * </p>
 * <h3>Y axis scaling</h3>
 * <p>
 * Since Y axis scaling does not affect the zoom behaviour, Y axis autoscaling is managed through QwtPlot
 * setAxisAutoscale() and setAxisScale().
 * </p>
 * @see setYLowerBound()
 * @see setYUpperBound()
 *
 * @see setXLowerBound()
 * @see setXUpperBound()
 *
 * The methods above disable x axis autoscale. To enable it again, you have to call setXAxisAutoscale(true).
 */
class QuPlotBase : public QwtPlot, public QuWidgetInterface
{
    Q_OBJECT

    Q_PROPERTY(bool xAxisAutoscale READ xAxisAutoscaleEnabled WRITE setXAxisAutoscaleEnabled)
    Q_PROPERTY(bool yAxisAutoscale READ yAxisAutoscaleEnabled WRITE setYAxisAutoscaleEnabled)
    Q_PROPERTY(double yUpperBound READ yUpperBound WRITE setYUpperBound)
    Q_PROPERTY(double yLowerBound READ yLowerBound WRITE setYLowerBound)
    Q_PROPERTY(double xUpperBound READ xUpperBound WRITE setXUpperBound)
    Q_PROPERTY(double xLowerBound READ xLowerBound WRITE setXLowerBound)
    Q_PROPERTY(double xAutoscaleMargin READ xAutoscaleMargin WRITE setXAutoscaleMargin)
    Q_PROPERTY(double yAutoscaleAdjustment READ yAutoscaleAdjustment WRITE setYAutoscaleAdjustment)
    Q_PROPERTY(bool titleOnCanvasEnabled  READ titleOnCanvasEnabled WRITE setTitleOnCanvasEnabled)
    Q_PROPERTY(bool yAxisLogScale READ yAxisLogScale WRITE setYAxisLogScale)
    Q_PROPERTY(bool zoomDisabled READ zoomDisabled WRITE setZoomDisabled)
    Q_PROPERTY(int dataBufferSize READ dataBufferSize WRITE setDataBufferSize)
    Q_PROPERTY(int refreshTimeout READ refreshTimeout WRITE setRefreshTimeout)

public:

    /* directly taken from QwtPlotCurve CurveStyle */
    enum CurveStyle  { NoCurve,  Lines,  Sticks,  Steps,  Dots,  UserCurve = 100 };

    /**
     * \brief The constructor of a simple plot with zooming and scrolling capabilities
     * @param parent the parent widget
     */
    QuPlotBase(QWidget *parent, bool openGL = false);
    
    /**
     * \brief The constructor of a simple plot with zooming and scrolling capabilities
     * @param parent the parent widget
     * @param title the title of the plot
     */
    QuPlotBase(const QwtText &title, QWidget *parent, bool openGL = false);
    
    virtual ~QuPlotBase();

    CurveStyle curvesStyle();

    bool titleOnCanvasEnabled();

    bool xAxisAutoscaleEnabled(QwtPlot::Axis axis = QwtPlot::xBottom);
    bool yAxisAutoscaleEnabled(QwtPlot::Axis axis = QwtPlot::yLeft);
    
    double yUpperBound(QwtPlot::Axis axis = QwtPlot::yLeft);
    double yLowerBound(QwtPlot::Axis axis = QwtPlot::yLeft);
    double xUpperBound(QwtPlot::Axis axis = QwtPlot::xBottom);
    double xLowerBound(QwtPlot::Axis axis = QwtPlot::xBottom);

    double defaultLowerBound(QwtPlot::Axis axisId = QwtPlot::yLeft) const;
    double defaultUpperBound(QwtPlot::Axis axisId = QwtPlot::yLeft) const;

    bool xAxisLogScale(QwtPlot::Axis axis = QwtPlot::xBottom);
    bool yAxisLogScale(QwtPlot::Axis axis = QwtPlot::yLeft);

    void setXAxisLogScale(bool l, QwtPlot::Axis axis = QwtPlot::xBottom);
    void setYAxisLogScale(bool l, QwtPlot::Axis axis = QwtPlot::yLeft);


    /** \brief returns false if the zoom is enabled (the default), true otherwise.
      *
      * @see setZoomDisabled
      */
    bool zoomDisabled() const;
    
    QList<QwtPlotCurve *>curves() const;
    
    void addCurve(const QString& curveName, QuPlotCurve *curve);

    /** \brief Removes the specified curve from the plot, deleting it.
     * @param curveName the name of the curve you want to remove.
     */
    void removeCurve(const QString &curveName);

    void addCurve(const QString &curveName);

    QuPlotCurve *curve(const QString& name);

    /** \brief returns the data buffer size.
     * @return the number of elements of the underlying data size.
     */
    int dataBufferSize();

    void hideMarker();

    void appendData(const QString &curveName, double *x, double *y, int size);

    int refreshTimeout() const;

    // QuWidgetInterface interface
    void update(const CuData &);

    void configure(const CuData &da);

    void setUpdateStrategy(QuWidgetUpdateStrategyI *updateStrategy);

    void setContextMenuStrategy(QuWidgetContextMenuStrategyI *ctx_menu_strategy);

    QuWidgetUpdateStrategyI* updateStrategy() const;

    QuWidgetContextMenuStrategyI* contextMenuStrategy() const;

    virtual QDialog *createConfigureDialog();

    virtual bool updateMarker();

    virtual bool updateScales();

    void resetZoom();

    bool inZoom() const;

    QuPlotComponent *getComponent(const QString& name) const;

    void registerComponent(QuPlotComponent *, const QString& name);

    QuPlotComponent *unregisterComponent(const QString& name);

    bool isOpenGL() const;

public slots:
    virtual void refresh();

    void setXAxisAutoscaleEnabled(bool en, QwtPlot::Axis axis);
    void setYAxisAutoscaleEnabled(bool en, QwtPlot::Axis axis);

    void setXTopAxisAutoscaleEnabled(bool autoscale);
    void setYRightAxisAutoscaleEnabled(bool autoscale);

    void setYLowerBound(double l);
    void setYUpperBound(double u);

    void setXLowerBound(double l);
    void setXUpperBound(double u);

    void setXAutoscaleMargin(double d);

    void setUpperBoundExtra(int axisId, double e);

    double upperBoundExtra(int axisId) const;

    double xAutoscaleMargin();

    void setYAutoscaleAdjustment(double d);

    double yAutoscaleAdjustment();

    void setTitleOnCanvasEnabled(bool en);

    void setCurveStyle(const QString &curveName, QwtPlotCurve::CurveStyle);

    void setDataBufferSize(int b);

    void setZoomDisabled(bool);

    void clearPlot();

    virtual void setData(const QString &curveName, const QVector<double> &xData, const QVector<double> &yData);

    virtual void setData(const QString &curveName, double *xData,  double *yData, int size);

    virtual void appendData(const QString &curveName, double x, double y);

    virtual void insertData(const QString &curveName, double *xData,  double *yData, int size, double default_y = 0.0);

    void setRefreshTimeout(int millis);

    void setDefaultBounds(double lb, double ub, Axis axisId = QwtPlot::yLeft);

    void restoreDefaultBounds(Axis axisId = QwtPlot::yLeft);

    void setAxisScaleDefaultEnabled(bool en, Axis axisId = QwtPlot::yLeft);

    void setXAxisAutoscaleEnabled(bool autoscale);
    void setYAxisAutoscaleEnabled(bool autoscale);

    QWidget *createCanvas(bool opengl);

    void setOpenGL(bool openGL);

protected:

    /** \brief returns the plot title.
     *
     * This is called if titleOnCanvasEnabled is true. Subclasses must reimplement this method
     * to provide a title drawn on the plot canvas. For an example, see TPlotLightMarker reimplementation.
     * This default implementation provides no title.
     */
    virtual QString plotTitle() { return QString(); }

    /** updates the title on the canvas */
    virtual void drawCanvas(QPainter *p);

    void mouseReleaseEvent(QMouseEvent *ev);

    void keyPressEvent(QKeyEvent *ke);

    void moveCurveToYRight(QwtPlotCurve *c, bool yr);

    virtual void init(bool opengl);

    void contextMenuEvent(QContextMenuEvent *);

    void us_to_ms(std::vector<double> & ts_us) const;

signals:
  void plotClicked(const QPoint& pt, QwtPlotCurve *closestCurve, double x, double y, const QPoint& curvePoint);

  /** \brief This signal is emitted when a new curve is added to the plot.
   *
   * This signal can be useful when you want to catch the creation and adding of a new curve
   * to the plot. You can then modify the characteristics of the curve right after it is added
   * to the plot.
   */
  void curveAdded(QuPlotCurve *);

  /** \brief This signal is emitted whenever a curve is removed from the plot via removeCurve.
   *
   * When a curve is detached from the plot, in the removeCurve() method, this signal is emitted.
   * Right after the signal is emitted, the curve is deleted. So be sure that you connect this signal
   * via a Qt::DirectConnection to your slot.
   */
  void curveRemoved(QuPlotCurve *);

  void markerTextChanged(const QString& yText, const QString& xText = "", double x = 0.0, double y = 0.0 );
  void markerVisibilityChanged(bool visible);

protected slots:
    void displayZoomHint();
    void eraseZoomHint();
    
    void plotZoomed(const QRectF &);

    void showMarker(const QPolygon &p);

    void print();

    /* save an image */
    void snapshot();

    void copyImage();

    virtual void configurePlot();

    /** \brief dumps the curves into a csv file.
     *
     * The columns are in the form xAxisValues1, yAxisValues1, ... xAxisValuesN, yAxisValuesN.
     */
    void saveData();

private:

    QuPlotBasePrivate *d;

    int findClosestPoint(QPoint p, QwtPlotCurve **closestCrv);
    void m_updateLabel(QwtPlotCurve *closestCurve, int closestPointIdx);
    void m_align_scales(); // 2.1
    void m_install_components();
    void m_canvas_conf(QWidget *canvasw);
};

#endif
