#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <cuthreadlistener.h>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget, public CuThreadListener
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;

    // CuThreadListener interface
public:
    void onProgress(int step, int total, const CuData &data);
    void onResult(const CuData &data);
    void onResult(const std::vector<CuData> &datalist);
    CuData getToken() const;
};
#endif // WIDGET_H
