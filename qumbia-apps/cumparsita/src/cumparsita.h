#ifndef Cumparsita_H
#define Cumparsita_H

#include <QWidget>
#include <qulogimpl.h>
#include <cucontrolsfactorypool.h>
#include <cumbiatango.h>
#include <cumbiaepics.h>


class CuData;
class CumbiaPool;

class Cumparsita : public QWidget
{
    Q_OBJECT

public:
    explicit Cumparsita(QWidget *parent = nullptr);
    ~Cumparsita();

private:
};

#endif // Cumparsita_H
