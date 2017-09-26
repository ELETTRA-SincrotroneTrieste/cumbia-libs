#ifndef CUTCWIDGETS_H
#define CUTCWIDGETS_H

#include <QWidget>
#include <qulogimpl.h>

class CumbiaTango;

namespace Ui {
class CuTCWidgets;
}

class CuData;

class CuTCWidgets : public QWidget
{
    Q_OBJECT

public:
    explicit CuTCWidgets(CumbiaTango *cut, QWidget *parent = 0);
    ~CuTCWidgets();

private slots:
    void configure(const CuData& d);

    void changeRefresh();

    void sourcesChanged();

    void unsetSources();

    void switchSources();

private:
    Ui::CuTCWidgets *ui;

    CumbiaTango *cu_t;
    QuLogImpl m_log_impl;
    CuLog *m_log;

    int m_switchCnt;

    QStringList m_oldSrcs;
    const int m_layoutColumnCount;
};

#endif // CUTCWIDGETS_H
