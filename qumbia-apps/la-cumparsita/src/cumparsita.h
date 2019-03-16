#ifndef Cumparsita_H
#define Cumparsita_H

#include <QWidget>
#include <qulogimpl.h>


class CuData;
class CumbiaPool;

class Cumparsita : public QWidget
{
    Q_OBJECT

public:
    explicit Cumparsita(QWidget *parent = nullptr);

    QObject *get_cumbia_customWidgetCollectionInterface() const;

    ~Cumparsita();

private:
};

#endif // Cumparsita_H
