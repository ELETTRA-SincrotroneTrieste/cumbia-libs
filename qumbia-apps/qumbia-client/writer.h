#ifndef ELEMENT_H
#define ELEMENT_H

#include <QWidget>

class Cumbia;
class CumbiaPool;
class CuControlsWriterFactoryI;
class CuControlsFactoryPool;
class QuButton;

class Writer : public QWidget
{
    Q_OBJECT
public:
    explicit Writer(QWidget *w, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool,  int dim, const QString& target);

    ~Writer();

signals:

public slots:
};

#endif // ELEMENT_H
