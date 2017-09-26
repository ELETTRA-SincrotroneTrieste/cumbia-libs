#ifndef CUTCWIDGETS_H
#define CUTCWIDGETS_H

#include <QWidget>
#include <qulogimpl.h>

class CumbiaEpics;

namespace Ui {
class CuTCWidgets;
}

class CuData;

class CuEpWidgets : public QWidget
{
    Q_OBJECT

public:
    explicit CuEpWidgets(CumbiaEpics *cuep, QWidget *parent = 0);
    ~CuEpWidgets();

private slots:
    void configure(const CuData& d);

    void changeRefresh();

    void sourcesChanged();

    void unsetSources();

    void switchSources();

private:
    Ui::CuTCWidgets *ui;

    CumbiaEpics *cu_ep;
    QuLogImpl m_log_impl;
    CuLog *m_log;

    int m_switchCnt;

    const int m_layoutColumnCount;
};

#endif // CUTCWIDGETS_H
