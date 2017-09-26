#ifndef QUTREALTIME_PLOT_H
#define QUTREALTIME_PLOT_H

#include "quspectrumplot.h"
#include <QDateTime>

class TRealtimePlotPrivate;

/** \brief A TPlotLightMarker specialized in managing real time commands.
 *
 * This class inherits from TPLotLightMarker and is tailored to work with commands.
 * Command syntax only is accepted by setSources, and the configuration dialog 
 * available through the right click menu allows the configuration of the real time
 * parameters that caracterize the real time commands.
 * The expected argout returned by the command is of type <em>spectrum</em>.
 */
class QuTRealtimePlot : public QuSpectrumPlot
{
  Q_OBJECT
  
  /* these are not designable, because they are intended to be used by the real time plot configuration 
   * widget (RtConfWidget). See public access methods for the notes.
   */
  Q_PROPERTY(int numberOfPoints READ numberOfPoints WRITE setNumberOfPoints DESIGNABLE false)
  Q_PROPERTY(int mode READ mode WRITE setMode DESIGNABLE false)
  Q_PROPERTY(int bunch1 READ bunch1 WRITE setBunch1 DESIGNABLE false)
  Q_PROPERTY(int bunch2 READ bunch2 WRITE setBunch2 DESIGNABLE false)
  Q_PROPERTY(QDateTime date1 READ date1 WRITE setDate1 DESIGNABLE false)
  Q_PROPERTY(QDateTime date2 READ date1 WRITE setDate2 DESIGNABLE false)
  
  public:
	
  QuTRealtimePlot(QWidget *parent, Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac);

  void setSource(const QString& s);

  void setSources(const QStringList &srcs);
  
  /** \brief returns the sources configured, together with information about frequency, mode and number of points
   *
   * Reimplemented from EPlotLightBase::plotTitle()
   */
   virtual QString plotTitle();
  
  public:
	
	/* access methods for the Q_PROPERTIES follow. Be sure, in the configuration dialogs or whenever
	 * you use these methods, that the setMode() must be called last, because it only triggers a real
	 * update of the qtangocore configured sources.
	 */
	
	int numberOfPoints() { return d_nPoints; }
	int mode() { return d_mode; }
	QDateTime date1() { return d_date1; }
	QDateTime date2() { return d_date2; }
	int bunch1() { return d_b1; }
	int bunch2() { return d_b2; }
	
	/** \brief changes the mode of the configured sources
	 *
	 * <strong>Note</strong>: call this <strong>after all the following</strong> methods,
	 * since setMode really changes the read mode of the plot (unsets the sources and 
	 * sets them again), taking the parameters from the other properties.
         * <p><ul>Available modes
	 * <li><strong>0</strong>: number of points mode: the configured number of points is read every refresh;</li>
	 * <li><strong>1</strong>: <em>bunch1 to bunch N</em> mode;</li>
         * <li><strong>2</strong>: <em>timestamp</em> mode.</li>
	 * </ul>
	 * The configuration parameters for the chosen mode (bunch1, bunch2, date1, date2) are Qt properties.
	 * They can be changed programmatically by the corresponding set... methods or by the right click 
	 * dialog.
	 * </p>
	 */
	void setMode(int m);

  public slots:
	void setNumberOfPoints(int n);
    void setDate1(const QDateTime &d1);
    void setDate2(const QDateTime &d2);
    void setBunch1(int b);
    void setBunch2(int b2);
    void updateParams();
	
	/* ^-- end of the Qt Properties access methods --^ */
	
   
  protected slots:
	
  private:
	
	/** \brief from the list of sources, extracts the default argin from the last one
	 *
	 * @return true if at least one source contains an argin
	 * @return false if no source contains an argin.
	 * 
	 * if at least one source contains an argin, this method returns true and saves the
	 * extracted argin in d_defaultArgin.
	 * If not, returns false and d_defaultArgin remains untouched
	 */
	bool extractDefaultArginFromSourcesList(const QStringList &sources);
	
	/** \brief given the argin (x,y), (x,y,z), ... updates mode, number of points, bunch1, bunchN, dates..
	 *
	 */
	void updatePropertiesFromArgin();
	
	/** 
	 * foreach string passed as argument in the list, `(x,y)' argin is removed
	 * and the sources will all be of type a/b/c->d
	 */
	void clearArginsFromSourcesList(QStringList &srcs);
	
	QString d_defaultArgin;
	
	  int d_nPoints, d_mode, d_b1, d_b2;
	  QDateTime d_date1, d_date2;

     TRealtimePlotPrivate *d_ptr;
};





#endif
