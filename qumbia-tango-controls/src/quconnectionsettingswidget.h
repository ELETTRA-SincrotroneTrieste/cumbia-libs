#ifndef QUCONNECTIONSETTINGSWIDGET_H
#define QUCONNECTIONSETTINGSWIDGET_H

#include <QWidget>
#include <cudata.h>

/** \brief a QWidget that emits a settingsChanged signal when tango connection
 *         settings are changed.
 *
 * Period and refresh mode can be changed.
 */
class QuConnectionSettingsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QuConnectionSettingsWidget(QWidget *w);

    ~QuConnectionSettingsWidget();

signals:
    void settingsChanged(const CuData& s);

public slots:
    void changeRefresh();

private:
};


#endif // QUCONNECTIONSETTINGSWIDGET_H
