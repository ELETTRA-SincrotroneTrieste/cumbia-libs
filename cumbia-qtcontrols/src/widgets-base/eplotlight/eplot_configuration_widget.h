#ifndef EPLOT_CONFIGURATION_WIDGET_H
#define EPLOT_CONFIGURATION_WIDGET_H

#include <QDialog>
#include <QSet>

class QuPlotCurve;
class QuPlotBase;

/*! @private
 *
 */
namespace  Ui {
class EPlotConfigurationWidget ;
}

/*! @private
 *
 */
class EPlotConfigurationWidget : public QWidget
{
  Q_OBJECT
  public:
	
	EPlotConfigurationWidget(QWidget *parent);

    virtual ~EPlotConfigurationWidget();

  signals:
	void xAxisAutoscaleEnabled(bool );
	void yAxisAutoscaleEnabled(bool );
	
	
	
	void yUpperBound(double);
	void xUpperBound(double);
	
	void xLowerBound(double);
	void yLowerBound(double);

	void xAutoscaleAdjustEnabled(bool);
	void yAutoscaleAdjustEnabled(bool);
	
	void yAutoscaleAdjustment(double);
	void xAutoscaleAdjustment(double);

	
  private:
    Ui::EPlotConfigurationWidget *ui;
    QuPlotBase *d_plot;
	QSet<QString> d_changedProperties;
	
	void init();
	void initAppearenceSettings();
	int getIndexOfProperty(const QString& name);
	double getPropertyAsDouble(const QString& name);
	void connectAppearenceWidgets();
	
 
  protected slots:
	void xAdjustmentChanged();
	void yAdjustmentChanged();
	void curveStyleChanged();
	void curveSelectionChanged(int);
	void writeProperty(const QString& name, const QVariant &val);
	void editCurveColor();
	void propertyChanged();
	void saveStyles();
	void apply();
    void restoreYScaleDefault();
};

/** @private
 * the EPlotLight configuration dialog.
 *
 * The parent used in the constructor must be a pointer to 
 * an EPlotLight object.
 */
class EPlotConfigurationDialog : public QDialog
{
  Q_OBJECT
  public:
	EPlotConfigurationDialog(QWidget *parent);
};

#endif
