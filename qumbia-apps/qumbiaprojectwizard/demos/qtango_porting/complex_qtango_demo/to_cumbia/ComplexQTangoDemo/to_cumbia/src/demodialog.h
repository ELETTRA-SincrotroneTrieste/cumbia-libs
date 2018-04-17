#ifndef DEMODIALOG_H
#define DEMODIALOG_H

#include <QDialog>

namespace Ui {
class DemoDialog;
}

class DemoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DemoDialog(QWidget *parent = 0);
    ~DemoDialog();

private:
    Ui::DemoDialog *ui;
};

#endif // DEMODIALOG_H

