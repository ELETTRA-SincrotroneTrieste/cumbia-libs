#ifndef CUINFODIALOG_H
#define CUINFODIALOG_H

#include <QDialog>

class CuInfoDialog : public QDialog
{
    Q_OBJECT
public:
    CuInfoDialog(QWidget *parent);

public slots:
    void onInfoRequested(QWidget *sender);
};

#endif // CUINFODIALOG_H
