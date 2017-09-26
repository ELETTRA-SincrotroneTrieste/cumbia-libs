#ifndef ELINEEDIT_H
#define ELINEEDIT_H

#include <QLineEdit>

class ELineEditPrivate;

class ELineEdit : public QLineEdit
{
    Q_OBJECT
public:

    ELineEdit(QWidget *parent);

private:

    ELineEditPrivate *d_ptr;
};



#endif
