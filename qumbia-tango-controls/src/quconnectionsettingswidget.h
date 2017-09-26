#ifndef QUCONNECTIONSETTINGSWIDGET_H
#define QUCONNECTIONSETTINGSWIDGET_H

#include <QWidget>
#include <cudata.h>

class QuPlotBase;

class QuConnectionSettingsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QuConnectionSettingsWidget(QuPlotBase *plot);

    ~QuConnectionSettingsWidget();

signals:
    void settingsChanged(const CuData& s);

public slots:
    void changeRefresh();

private:
    QuPlotBase *m_plot;
};


#endif // QUCONNECTIONSETTINGSWIDGET_H
