#ifndef TTABLEWIDGET_H
#define TTABLEWIDGET_H

#include <QTableWidget>
#include <QStringList>
#include <qtangocore.h>
#include <com_proxy_reader.h>
#include <qtango_wcommon.h>

/* TTableWidget is a reader and also a QTangoWidget common, to benefit from show/hide event system
 */
class TTableWidget : public QTableWidget, public QTangoComProxyReader, public QTangoWidgetCommon
{
Q_OBJECT

public:
    TTableWidget(QWidget * = 0);

    void setLabels(QStringList l){ _labels = l; };
    QStringList labels(){ return _labels; };

    void setDescriptions(QStringList d){ _descriptions = d; };
    QStringList descriptions(){ return _descriptions; };
	
	QTangoCommunicationHandle *qtangoComHandle() const { return QTangoComProxyReader::qtangoComHandle(); }

protected slots:
    void refresh(const TVariant &v);
	void configure(const TangoConfigurationParameters *cp);

protected:
    QStringList _labels;
    QStringList _descriptions;
    QVector<bool> oldData;

};

#endif
