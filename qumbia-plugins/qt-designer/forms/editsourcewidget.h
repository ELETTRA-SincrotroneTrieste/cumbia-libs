#ifndef EDITSOURCEWIDGET_H
#define EDITSOURCEWIDGET_H

#include <QWidget>

#include "ui_editsourcewidget.h"

class CuFormulaParserI;

class EditSourceWidget : public QWidget
{
    Q_OBJECT
public:
    explicit EditSourceWidget(int index, QWidget *parent = nullptr);

    QString source() const;

    void setSource(const QString& s);

    bool checkSource();

signals:

public slots:
protected slots:

    void loadFromJs();
    void onFormulaEnabled(bool en);
    void expressionValid();

private:
    int m_index;
    CuFormulaParserI *m_formulaParserI;

    Ui::EditSourceWidget ui;
};

#endif // EDITSOURCEWIDGET_H
