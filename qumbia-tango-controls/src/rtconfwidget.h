#ifndef RTCONFWIDGET_H
#define RTCONFWIDGET_H

#include "ui_rtconfwidget.h"

class RtConfWidgetPrivate;
class QuTRealtimePlot;

/*! \brief the *real time plot* configuration widget has not yet been made compatible
 *         with cumbia-qtcontrols plots
 *
 * This configuration widget cannot be currently used
 */
class RtConfWidget : public QWidget
{
  Q_OBJECT
  public:
      RtConfWidget(QuTRealtimePlot *parent);
	  
  private slots:
	void modeChanged(int m); 
	void apply();
	
  private:
	/* initializes widgets from Qt metaProperties of the parent */
	void init();
	
  Ui_RtConfWidget ui;
  QWidget *d_plotWidget;

  RtConfWidgetPrivate *d_ptr;
};

#endif
