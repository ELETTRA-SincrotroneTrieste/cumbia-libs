#ifndef Cuengineaccessor_H
#define Cuengineaccessor_H

#include <QWidget>

// cumbia
#include <qulogimpl.h>
#include <cucontrolsfactorypool.h>
class CumbiaPool;
// cumbia

namespace Ui {
class Cuengineaccessor;
}

class CuengineaccessorTest : public QWidget
{
    Q_OBJECT

public:
    explicit CuengineaccessorTest(QWidget *parent = 0);
    ~CuengineaccessorTest();

private:
    Ui::Cuengineaccessor *ui;

    // cumbia
    QuLogImpl m_log_impl;
    // cumbia
};

#endif // Cuengineaccessor_H
