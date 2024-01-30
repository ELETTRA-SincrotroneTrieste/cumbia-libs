#include "snapshot.h"
#include <QPixmap>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QString>
#include <QClipboard>
#include <QApplication>

bool PlotSnapshot::grab(QWidget *w)
{
  bool ret = true;
  QPixmap pix = w->grab();
  if(!pix.isNull())
  {
	QString fileName = QFileDialog::getSaveFileName(w, "Save picture as...", QDir::homePath(), "Images (*.png *.xpm *.jpg)");
	if(!fileName.isEmpty())
	{
	  ret = pix.save(fileName);
	  if(!ret)
		QMessageBox::critical(w, "Error saving image", "Failed to save image in the specified format");
	}
  }
  return ret;
}

void PlotSnapshot::copy(QWidget *w)
{
  QPixmap pix = w->grab();
  if(!pix.isNull())
  {
	QClipboard *clipboard = QApplication::clipboard();
	clipboard->setPixmap(pix);
  }
}
