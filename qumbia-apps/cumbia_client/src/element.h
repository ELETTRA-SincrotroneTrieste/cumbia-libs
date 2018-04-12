#ifndef ELEMENT_H
#define ELEMENT_H

#include <QWidget>

class Element : public QWidget
{
    Q_OBJECT
public:
    explicit Element(QWidget *parent = nullptr);

signals:

public slots:
};

#endif // ELEMENT_H