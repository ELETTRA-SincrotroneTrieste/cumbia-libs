#ifndef TTREEWIDGET_H
#define TTREEWIDGET_H

#include <QTreeWidget>
#include <QStringList>
#include <qtangocore.h>
#include <com_proxy_reader.h>
#include <qtango_wcommon.h>

/* QTangoWidgetCommon to benefit from show/hide event system */
class TTreeWidget : public QTreeWidget, public QTangoComProxyReader, public QTangoWidgetCommon
{
Q_OBJECT

public:
    TTreeWidget(QWidget * = 0);

    void setLabels(QStringList l){ _labels = l; };
    QStringList labels(){ return _labels; };

    void setDescriptions(QStringList d){ _descriptions = d; };
    QStringList descriptions(){ return _descriptions; };

    unsigned int dataSize(){ return _dataSize; };

signals:
    void acknowledgeAll();
    void acknowledgeSelected();
    void newAlarmArrived();

protected slots:
    void refresh(const TVariant &v);
	void configure(const TangoConfigurationParameters *cp);
    void ackSelected();
    void ackAll();

    void filter(const QString& text);

protected:
    void contextMenuEvent(QContextMenuEvent *);
    QStringList _labels;
    QStringList _descriptions;
    unsigned int _dataSize;
    QVector<double> oldData;

    void showEvent(QShowEvent *e);
    void hideEvent(QHideEvent *);

private:
    QString mCurrentFilter;
    int mDecolorCount;
    QColor mAlarmColor;
    double mLastTimestampSeen;

    int tmpCounter;
};

#endif
