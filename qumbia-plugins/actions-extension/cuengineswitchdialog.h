#ifndef CUENGINESWITCHDIALOG_H
#define CUENGINESWITCHDIALOG_H

#include <QDialog>

class CuData;
class CuContextI;
class CuEngineSwitchDialogPrivate;

class CuEngineSwitchDialog : public QDialog
{
    Q_OBJECT
public:
    CuEngineSwitchDialog(QWidget *parent);
    virtual ~CuEngineSwitchDialog();
    void exec(const CuData& in, const CuContextI *ctxi);
    QObject *root_obj(QObject *leaf);
public slots:
    void switchEngine(bool checked);
private:
    CuEngineSwitchDialogPrivate * d;
    void m_resizeToMinimumSizeHint();
};

#endif // CUENGINESWITCHDIALOG_H
