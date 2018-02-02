#ifndef PLOT_SNAPSHOT_H
#define PLOT_SNAPSHOT_H

#include <QWidget>

/*! @private */
class PlotSnapshot : public QObject
{
  public:
    PlotSnapshot() {}
	void copy(QWidget *w);
	bool grab(QWidget *w);
};


#endif
